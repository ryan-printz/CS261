#include "ProtoConnection.h"
#include "SequenceNumber.h"

#include <sstream>

const static uint CONNECTION_MESSAGE = 0x5e015d13;
const static uint KEEP_ALIVE_MESSAGE = 0x1d5e03e7;
const static uint DISCONNECT_MESSAGE = 0xf70f8c3b;

ProtoConnection::ProtoConnection()
	: m_socket(nullptr), m_connected(false), m_useFlowControl(true), m_local(1)
{
	m_stats.m_ackedPackets = m_stats.m_lostPackets 
						   = m_stats.m_receivedPackets 
						   = m_stats.m_sentPackets = 0;
	m_stats.m_roundTripTime = m_stats.m_upBandwidth
							= m_stats.m_downBandwith = 0.0f;

	m_keepAliveInterval = 0;
	m_timeout = 0;

	m_idleTimer = 0.0f;
	m_modeTimer = 0.0f;
	m_unpenaltyTimer = 0.0f;

	m_penaltyTimer = 4.0f;
	m_sendRate = 0.01f;
	m_flowTimer = 0.f;
}

// socket and the destination ip:port pair.
// the socket should be bound on the server end.
bool ProtoConnection::accept(Socket * socket)
{
	if( m_connected )
		return false;

	m_socket = socket;

	uint connectionMessage = 0;
	if( receive((ubyte*)&connectionMessage, sizeof(uint)) < sizeof(uint) 
		|| connectionMessage != CONNECTION_MESSAGE )
		return false;

	m_server = true;
	m_connected = true;

	return true;
}

bool ProtoConnection::connect(const char * IP, uint port)
{
	m_socket = new Socket();

	m_socket->initializeUDP(Socket::localIP(), port, AF_INET);
	m_socket->setBlocking(false);

	m_connection.sin_family = AF_INET;
	m_connection.sin_port = htons(port);
	m_connection.sin_addr.s_addr = inet_addr(IP);

	m_server = false;
	m_connected = true;
	
	send((ubyte*)&CONNECTION_MESSAGE, sizeof(uint));

	return true;
}

bool ProtoConnection::disconnect()
{
	send((ubyte*)&DISCONNECT_MESSAGE, sizeof(uint));
	return true;
}

bool ProtoConnection::cleanup()
{
	if( !m_server ) 
	{
		m_socket->cleanUp();
		delete m_socket;
	}

	return true;
}

int ProtoConnection::send(ubyte * buffer, uint len, ubyte flags)
{
	if( !m_useFlowControl )
		return noFlowSend(buffer, len, flags);

	FlowPacket fp;
	memcpy( fp.m_buffer, buffer, len );

	fp.m_size = len;
	fp.m_flags = flags;

	m_flowControl.insert(fp);

	return len;
}

// send a packet to the "connected" 
int ProtoConnection::noFlowSend(ubyte * buffer, uint len, ubyte flags)
{
	ubyte packet[256];

	ProtoHeader header;
	header.m_sequence = m_local;
	header.m_ack	  = m_remote;
	header.m_acks	  = makeAck();
	header.m_flags	  = flags;

	memcpy( packet, &header, sizeof(ProtoHeader) );
	memcpy( packet + sizeof(ProtoHeader), buffer, len );

	if( m_socket->send(packet, len + sizeof(ProtoHeader), &m_connection) != len + sizeof(ProtoHeader) )
		return 0;
	printf("Sending!\n");
	if( flags & ProtoHeader::PROTO_HIGH )
	{
		ResendPacket repack;

		memcpy( repack.m_packet, &header.m_sequence, sizeof(SequenceNumber) );
		memcpy( repack.m_packet + sizeof(SequenceNumber), buffer, len );

		repack.m_size = len + sizeof(SequenceNumber);
		repack.m_sequence = header.m_sequence;

		m_resend.insert( repack );
	}

	// store info about this packet for stats.
	PacketInfo info;
	info.m_sequence = m_local;
	info.m_time		= 0.0f;
	info.m_size		= len;

	m_sentPackets.push_back(info);
	m_unackedPackets.push_back(info);

	++m_local;
	++m_stats.m_sentPackets;

	return len;
}

// receives a datagram.
int ProtoConnection::receive(ubyte * buffer, uint len, int drop)
{
	ubyte packet[256];
	uint headerSize = sizeof(ProtoHeader);

	NetAddress from;
	int received = m_socket->receive(packet, 256, &from);

	// no packet was received
	// or the packet was not from this protocol.
	if( received == -1 || !reinterpret_cast<ProtoHeader*>(packet)->valid() )
		return -1;

	// lost connection.
	else if( received == 0 )
	{
		m_connected = false;
		return 0;
	}

	// Whoops, drop the packet.
	if( drop == -1 )
		return drop;

	// pull out the header.
	// adjust the received size appropriately.
	ProtoHeader * header = reinterpret_cast<ProtoHeader*>(packet);
	received -= headerSize;

	m_idleTimer = 0.0f;
	++m_stats.m_receivedPackets;

	// drop duplicate packets.
	if( m_receivedPackets.has( header->m_sequence ) )
		return -1;

	if( header->m_flags & ProtoHeader::PROTO_RESENT )
	{
		SequenceNumber * old = reinterpret_cast<SequenceNumber*>(packet + headerSize);
		received -= sizeof(SequenceNumber);

		PacketInfo resentInfo;
		resentInfo.m_sequence = *old;
		resentInfo.m_time	  = 0.0f;
		resentInfo.m_size	  = sizeof(SequenceNumber);

		m_receivedPackets.insert(resentInfo);
	}

	PacketInfo info;
	info.m_sequence	= header->m_sequence;
	info.m_time		= 0.0f;
	info.m_size		= received;

	// store the packet to generate acks later.
	m_receivedPackets.push_back( info );

	// update the remote sequence number.
	if( header->m_sequence > m_remote )
		m_remote = header->m_sequence;

	// check out what the packet ack'd
	useAck(header->m_ack, header->m_acks, header->m_flags & ProtoHeader::PROTO_RESENT);

	// don't return keep alive packets.
	// handle disconnect messages.
	if( received == sizeof(uint) )
		if( *(uint*)(packet + headerSize) == KEEP_ALIVE_MESSAGE )
		{
			return -1;
		}
		else if( *(uint*)(packet + headerSize) == DISCONNECT_MESSAGE )
		{
			m_connected = false;
			return 0;
		}

	memcpy(buffer, packet + headerSize, received);

	return received;
}

void ProtoConnection::updateFlowControl(float dt)
{
	m_flowTimer += dt;

	// if send rate > 10fps
	if( m_sendRate < 0.01f )
	{
		if( m_stats.m_roundTripTime > m_goodRoundTripTime )
		{
			// degrade the connection.
			if( (m_sendRate *= 2.0f) > 0.01f )
				m_sendRate = 0.01f;

			if( m_modeTimer > 10.0f && m_penaltyTimer < 60.0f )
				if( (m_penaltyTimer *= 2.0f) > 60.0f )
					m_penaltyTimer = 60.0f;

			m_modeTimer = 0.0f;
			m_unpenaltyTimer = 0.0f;
		}
		else
		{
			m_modeTimer += dt;
			if( (m_unpenaltyTimer += dt) > 10.0f && m_penaltyTimer > 1.0f )
			{
				if( (m_penaltyTimer *= 0.5f) < 1.0f )
					m_penaltyTimer = 1.0f;
				m_unpenaltyTimer = 0.0f;
			}
		}
	}
		
	// if send rate < 30fps
	if( m_sendRate > 0.0032f )
	{
		if( m_stats.m_roundTripTime <= m_goodRoundTripTime )
			m_modeTimer += dt;
		else
			m_modeTimer = 0.0f;

		if( m_modeTimer > m_penaltyTimer )
		{
			m_modeTimer = 0.0f;
			m_penaltyTimer = 0.0f;

			if( (m_sendRate *= 0.5f) < 0.0032f )
				m_sendRate = 0.0032f;
		}
	}

	while( m_flowTimer > (1.0f / m_sendRate) / 1000.f && !m_flowControl.empty() )
	{
		noFlowSend(m_flowControl.back().m_buffer, m_flowControl.back().m_size, m_flowControl.back().m_flags);
		m_flowControl.pop_back();

		m_flowTimer -= 1.0f/m_sendRate;
	}
}

void ProtoConnection::update(float dt)
{
	// get a little bit closer to timing out.
	m_idleTimer += dt;

	if( m_keepAliveInterval && !((int)m_idleTimer % m_keepAliveInterval) )
	{
		send((ubyte*)&KEEP_ALIVE_MESSAGE, sizeof(uint));
	}

	if( m_timeout && m_idleTimer > m_timeout )
	{
		m_connected = false;
	}

	// advance all the times in the packet queues.
	m_resend.advance(dt);
	m_sentPackets.advance(dt);
	m_ackedPackets.advance(dt);
	m_unackedPackets.advance(dt);
	m_receivedPackets.advance(dt);

	if( m_useFlowControl )
		updateFlowControl(dt);

	// update each set of packets.
	// remove all the sent packets that have been around for longer than timeout.
	while( !m_sentPackets.empty() && m_sentPackets.front().m_time > m_timeout )
		m_sentPackets.pop_front();

	// remove all the sent packets that have been around for longer than 2 * timeout
	// used to determine round trip time.
	while( !m_ackedPackets.empty() && m_sentPackets.front().m_time > 2 * m_timeout )
		m_ackedPackets.pop_front();

	// if a packet takes more than timeout time to be acked, it has been dropped.
	while( !m_unackedPackets.empty() && m_unackedPackets.front().m_time > m_timeout )
	{
		m_unackedPackets.pop_front();
		++m_stats.m_lostPackets;
	}

	if( !m_receivedPackets.empty() )
	{
		// maintain the last 33 packets received (by sequence number)
		const SequenceNumber latest = m_receivedPackets.back().m_sequence;
		const SequenceNumber oldest = (uint)latest >= 34 ? (latest - 34) : std::numeric_limits<uint>::max() - (34 - latest);

		// remove the received packets that have lower sequence numbers than the latest 33 packets.
		while( !m_receivedPackets.empty() && m_receivedPackets.front().m_sequence <= oldest )
			m_receivedPackets.pop_front();
	}

	// resend high priority packets if they still haven't been acked.
	for(auto resend = m_resend.begin(); resend != m_resend.end(); ++resend)
		if( resend->m_time > 1.5f * m_timeout )
		{
			send(resend->m_packet, resend->m_size, ProtoHeader::PROTO_RESENT);
			resend->m_time = 0.0f;
		}
}

void ProtoConnection::updateStats()
{
	int upBPS = 0;
	for(auto upPacket = m_sentPackets.begin(); upPacket != m_sentPackets.end(); ++upPacket)
		upBPS += upPacket->m_size;

	int downBPS = 0;
	for(auto downPacket = m_receivedPackets.begin(); downPacket != m_receivedPackets.end(); ++downPacket)
		downBPS += downPacket->m_size;

	// bytes per second
	upBPS /= m_timeout;
	downBPS /= m_timeout;

	// bits per millisecond.
	m_stats.m_upBandwidth = upBPS * 8 / 1000.0f;
	m_stats.m_downBandwith = downBPS * 8 / 1000.0f;
}

std::string ProtoConnection::connectionInfo() const
{
	std::stringstream info;
	info << "UDP Connection:";

	if( m_connected )
		info << " " << m_socket->ipAddress() << ":" << m_socket->port() << std::endl;
	else
		info << " not connected.";

	info << std::endl << "-----------------------" << std::endl;
	info << m_stats << std::endl;

	return info.str();
}

// builds an ack pack.
uint ProtoConnection::makeAck()
{
	uint ack = 0;
	for(auto it = m_receivedPackets.begin(); it != m_receivedPackets.end(); ++it)
	{
		if( it->m_sequence == m_remote || it->m_sequence > m_remote )
			break;

		ubyte bitIndex = getBitIndex(it->m_sequence, m_remote);
		if( bitIndex < 32 )
			ack |= 1 << bitIndex;
	}

	return ack;
}

void ProtoConnection::useAck(SequenceNumber ack, uint ackPack, bool resent)
{
	if( m_unackedPackets.empty() )
		return;

	if( resent && m_resend.has(ack) )
		for(auto rp = m_resend.begin(); rp != m_resend.end(); ++rp)
			if( rp->m_sequence == ack )
				m_resend.erase(rp);

	auto packet = m_unackedPackets.begin();
	while( packet != m_unackedPackets.end() )
	{
		bool packetAcked = false;
		
		if(packet->m_sequence == ack)
			packetAcked = true;
		else if( packet->m_sequence <= ack )
		{
			int bitIndex = getBitIndex(packet->m_sequence, ack);
			if( bitIndex < 32 )
				packetAcked = (ackPack >> bitIndex) & 1;
		}

		if( packetAcked )
		{
			// update stats.
			++m_stats.m_ackedPackets;
			updateRTT(packet->m_time);

			// move the packet to the acked queue
			// remove it from unacked.
			// set it as acked this update.
			m_ackedPackets.insert(*packet);
			m_acks.push_back(packet->m_sequence);
			packet = m_unackedPackets.erase( packet );
		}
		else
			++packet;
	}
}

// updates the round trip time with a running average.
void ProtoConnection::updateRTT(float time)
{
	m_stats.m_roundTripTime += (time - m_stats.m_roundTripTime) * 0.1f;
}

ubyte ProtoConnection::getBitIndex(const SequenceNumber & lhs, const SequenceNumber & rhs)
{
	if( lhs > rhs )
		return rhs + (std::numeric_limits<uint>::max() - lhs);
	else
		return rhs - 1 - lhs;
}

bool ProtoConnection::connected() const
{
	return m_connected;
}

std::ostream & operator<<(std::ostream & os, const ConnectionStats & stats)
{
	std::ostream::sentry ok(os);
	os << "up/down bandwidth: " << stats.m_upBandwidth << "/" << stats.m_downBandwith << std::endl;
	os << "ping: " << stats.m_roundTripTime << std::endl; 
	os << "lifetime sent: " << stats.m_sentPackets << std::endl;
	os << "lifetime acked: " << stats.m_ackedPackets << std::endl;
	os << "lifetime received: " << stats.m_receivedPackets << std::endl; 
	os << "lifetime lost: " << stats.m_lostPackets;
	return os;
}

bool operator==(const FlowPacket &lhs, const FlowPacket &rhs)
{
	const ubyte highlownorm = ProtoConnection::ProtoHeader::PROTO_HIGH |
							   ProtoConnection::ProtoHeader::PROTO_LOW | 
							   ProtoConnection::ProtoHeader::PROTO_NORMAL;
	ubyte priority = lhs.m_flags & rhs.m_flags;
	return priority & highlownorm ? true : false;
}

bool operator!=(const FlowPacket &lhs, const FlowPacket &rhs)
{
	return !operator==(lhs, rhs);
}

bool operator >(const FlowPacket &lhs, const FlowPacket &rhs)
{
	// lhs has greater priority iff:
	// lhs is high priority && rhs is NOT high priority.
	// lhs is NOT low priority && rhs is low priority.
	return ((lhs.m_flags & ProtoConnection::ProtoHeader::PROTO_HIGH) && !(rhs.m_flags & ProtoConnection::ProtoHeader::PROTO_HIGH))
		|| (!(lhs.m_flags & ProtoConnection::ProtoHeader::PROTO_LOW) && (rhs.m_flags & ProtoConnection::ProtoHeader::PROTO_LOW));
}

bool operator <(const FlowPacket &lhs, const FlowPacket &rhs)
{
	return operator>(rhs, lhs);
}

bool operator>=(const FlowPacket &lhs, const FlowPacket &rhs)
{
	return !operator<(lhs, rhs);
}

bool operator<=(const FlowPacket &lhs, const FlowPacket &rhs)
{
	return !operator>(lhs, rhs);
}
