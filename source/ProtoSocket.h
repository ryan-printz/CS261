#pragma once

#include "Socket.h"
#include "ProtoHeader.h"
#include <list>

class ProtoSocket : public Socket
{
public:
	const static uint MAX_PACKET_SIZE =	1024;

	struct Packet
	{
		NetAddress from;
		ubyte	   packet[MAX_PACKET_SIZE];
		uint	   packetSize;
	};

public:
	class ProtoSocket()
		: m_packetSize(MAX_PACKET_SIZE), m_isInitialized(false), m_header(nullptr)
	{};

	virtual ~ProtoSocket()
	{};

	bool initializeProto(const char * ipAddress, uint port, uint family = AF_INET)
	{
		if( !m_isInitialized ) 
			return m_isInitialized = Socket::initializeUDP(ipAddress, port, family);
		else	
			return false;
	}

	bool listen(ubyte backlog = 10)
	{
		// get connections if an attempt exists. 
		// (and is the next packet if you're nonblocking)
		do 
		{ receiveSort(); }
		while(m_isBlocking && m_connectionAttempts.empty());

		while( m_connectionAttempts.size() > backlog )
			m_connectionAttempts.pop_front();

		return true;
	}

	// returns a new protosocket that is initialized to the 
	// first available attempted connection
	ProtoSocket acceptProto()
	{
		ProtoSocket accepted;

		if( m_connectionAttempts.empty() )
			return accepted;

		accepted.m_socket = m_socket;
		accepted.m_address = m_connectionAttempts.front();
		accepted.m_isConnected = true;
		accepted.m_isBlocking = m_isBlocking;

		m_connectionAttempts.pop_front();

		return accepted;
	}

	bool connect()
	{
		connect(&m_address);
	}

	bool connect(const NetAddress * address)
	{
		return (sizeof(uint) == Socket::send((ubyte*)&ProtoHeader::CONNECTION_MESSAGE, sizeof(uint), const_cast<NetAddress*>(address)));
	}

	int send(ubyte * buffer, uint len)
	{
		return send(buffer, len, &m_address);
	}

	void setProtoHeader(ProtoHeader * header)
	{
		m_header = header;
	}

	ProtoHeader getProtoHeader()
	{
		return m_lastProtoHeader;
	}

	int send(ubyte * buffer, uint len, const NetAddress * address)
	{
		ubyte packet[MAX_PACKET_SIZE];

		memcpy(packet, &m_header, sizeof(ProtoHeader));
		memcpy(packet + sizeof(ProtoHeader), buffer, std::min(len, MAX_PACKET_SIZE - sizeof(ProtoHeader));

		return Socket::send(buffer, std::min(len+sizeof(ProtoHeader), MAX_PACKET_SIZE));
	}

	int receive(ubyte * buffer, uint len)
	{
		std::list<Packet>::iterator packet;
		
		do {
			receiveSort();

			packet = m_received.begin();
			for( ; packet != m_received.end(); ++packet  )
				if( packet->from.sin_port == m_address.sin_port 
					&& packet->from.sin_addr.s_addr == m_address.sin_addr.s_addr )
					break;
		} while (m_isBlocking && packet == m_received.end());

		if( packet == m_received.end() )
			return -1;

		uint size = std::min(len, packet->packetSize - sizeof(ProtoHeader));
		memcpy(&m_lastProtoHeader, packet->packet, sizeof(ProtoHeader));
		memcpy(buffer, packet->packet + sizeof(ProtoHeader), size);

		m_received.erase(packet);

		return size;
	}

	int receive(ubyte * buffer, uint len, NetAddress * address )
	{
		int recvd = receive(buffer, len);

		if( recvd > 0 )
			*address = m_address;

		return recvd;
	}

private:
	bool initializeTCP(const char * ipAddress, uint port, uint family = AF_INET);
	bool initializeUDP(const char * ipAddress, uint port, uint family = AF_INET);
	Socket accept();

	// receive packets and sort them as necessary.
	void receiveSort()
	{
		Packet p;

		p.packetSize = receive(p.packet, m_packetSize, &p.from);

		// nothing to receive
		if( p.packetSize == SOCKET_ERROR && m_error == WSAEWOULDBLOCK )
			return;

		// client disconnected.
		else if( p.packetSize == 0 )
		{
			m_disconnected.emplace_back( p.from );
			return;
		}

		// not from a proto send
		else if( p.packetSize < sizeof(ProtoHeader) || !reinterpret_cast<ProtoHeader*>(p.packet)->valid() )
			return;

		// connection attempt
		else if ( p.packetSize - sizeof(ProtoHeader) == sizeof(uint) 
			   && ProtoHeader::CONNECTION_MESSAGE == *(p.packet + sizeof(ProtoHeader)) )
		{
			m_connectionAttempts.emplace_back(p.from);
			return;
		}

		// disconnection
		else if ( p.packetSize - sizeof(ProtoHeader) == sizeof(uint) 
			   && ProtoHeader::DISCONNECT_MESSAGE == *(p.packet + sizeof(ProtoHeader)) )
		{
			m_disconnected.emplace_back(p.from);
			return;
		}
		
		// else it's just a packet.
		m_received.emplace_back(m_received);
	}

	std::list<NetAddress> m_connectionAttempts;
	std::list<NetAddress> m_disconnected;
	std::list<Packet>     m_received;

	ProtoHeader * m_header;
	ProtoHeader m_lastProtoHeader;
	uint m_packetSize;
	bool m_isInitialized;
};