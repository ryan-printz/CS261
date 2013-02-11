#include "ProtoConnection.h"

ProtoConnection::ProtoConnection()
	: m_socket(nullptr), m_isClient(false), m_connected(false)
{}

int ProtoConnection::addProtoHeader(ubyte * message, uint size, uint priority, bool requestAck)
{
	msgHeader header;
	header.flags = 0;

	if(requestAck)
		header.flags |= MSG_REQUESTACK;

	switch( priority )
	{
	case 0:
		header.flags |= MSG_HIGH_PRIORITY;
		break;

	case 2:
		header.flags |= MSG_LOW_PRIORITY;
		break;
	};

	header.length = size;

	// shift the message right by the size of the header.
	// the overflow is discarded.
	for(int i = max(size + sizeof(msgHeader), s_maxSize) - 1; i >= sizeof(msgHeader); --i)
		message[i] = message[i - sizeof(msgHeader)];

	// attach the header.
	memcpy(message, &header, sizeof(msgHeader));

	return size + sizeof(msgHeader);
}

int ProtoConnection::makeAck(ubyte * buffer)
{
	msgHeader header;

	header.flags = MSG_ACK;
	header.length = 1;
	header.sequenceNumber = m_sequenceNumber;

	memcpy(buffer, &header, sizeof(header));

	int * acks = (int*)(buffer + sizeof(header));
	*acks = 0;
}

// socket and the destination ip:port pair.
// the socket should be bound on the server end.
bool ProtoConnection::connect(Socket * socket, char * IP, uint port)
{
	m_socket = socket;

	m_connection.sin_family = AF_INET;
	m_connection.sin_port = htons(port);
	m_connection.sin_addr.s_addr = inet_addr(IP);

	return true;
}

bool ProtoConnection::connect(char * IP, uint port)
{
	m_socket = new Socket();

	m_socket->initializeUDP(Socket::localIP(), port, AF_INET);

	m_connection.sin_family = AF_INET;
	m_connection.sin_port = htons(port);
	m_connection.sin_addr.s_addr = inet_addr(IP);

	m_isClient = true;

	return true;
}

bool ProtoConnection::cleanUp()
{
	if( m_isClient ) 
	{
		m_socket->cleanUp();
		delete m_socket;
	}

	return true;
}

// send a packet to the "connected" 
// needs to have a header, or it will cause errors with the received 
// packet.
int ProtoConnection::send(ubyte * buffer, uint len)
{
	msgHeader * header = (msgHeader*)buffer;

	if(header->flags & MSG_REQUESTACK)
	{
		msgInfo info;
		info.length = len;

		// copy the message to the sent buffer, then mark it as a resent packet.
		memcpy(info.message, buffer, len);
		reinterpret_cast<msgHeader*>(&info.message)->flags | MSG_RESEND;

		m_resend.emplace(std::make_pair(m_sequenceNumber, info));

		header->sequenceNumber = m_sequenceNumber;
	}

	return m_socket->send(buffer, len, &m_connection);
}

int ProtoConnection::update(float d)
{
	for (auto message = m_resend.begin(); message != m_resend.end(); ++message)
	{
		m_socket->send(message->second.message, message->second.length, &m_connection);
	}
}

// receives a datagram.
// there is no logic for the connection, 
// receives all data sent to the port, regardless of 
// the sender.
int ProtoConnection::receive(ubyte * buffer, uint len)
{
	int result = m_socket->receive(buffer, len, nullptr);

	msgHeader * header = (msgHeader*)buffer;

	ubyte responseBuffer[256];
	uint responseSize = 0;
	if(header->flags & MSG_RESEND)
	{
		// check if this is actually a duplicate resend.
		// if it is, ack it, then break.
		// otherwise, mark it as recieved, then fall through.

		auto packet = m_missedPackets.begin();
		for(; packet != m_missedPackets.end() && *packet != header->sequenceNumber; ++packet);

		if(packet == m_missedPackets.end())
		{
			responseSize = makeAck(responseBuffer);
			m_socket->send(responseBuffer, responseSize, &m_connection);

			return result;
		}

		m_missedPackets.erase(packet);
	}

	if(header->flags & MSG_REQUESTACK)
	{
		// update the sequence number
		// mark any missing packets in between.
		// if the packet is sufficiently high priority.
		if(!(header->flags & MSG_LOW_PRIORITY))
			for(int i = m_sequenceNumber; i < header->sequenceNumber; ++i)
				m_missedPackets.push_back(i);

		m_sequenceNumber = header->sequenceNumber;
		
		// send an ack.
		responseSize = makeAck(responseBuffer);
		m_socket->send(responseBuffer, responseSize, &m_connection);
	}
}

std::string ProtoConnection::connectionInfo() const
{
	std::string info = "UDP Connection:";

	if( !m_socket )
	{
		char buffer[16];
		_itoa_s<16>(ntohs(m_connection.sin_port), buffer, 10);
		info.append(" ").append(inet_ntoa(m_connection.sin_addr)).append(":").append(buffer);
	}
	else
		info + " not connected.";

	return info;
}
