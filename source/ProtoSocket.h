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
	ProtoSocket& operator=(const ProtoSocket & rhs);
	ProtoSocket();
	virtual ~ProtoSocket();

	bool initializeProto(const char * ipAddress, uint port, uint family = AF_INET);
	bool listen(ubyte backlog = 10);

	// returns a new protosocket that is initialized to the 
	// first available attempted connection
	ProtoSocket acceptProto();

	bool connect();
	bool connect(const NetAddress * address);
	int send(ubyte * buffer, uint len);
	void setProtoHeader(ProtoHeader * header);
	ProtoHeader getProtoHeader();

	int send(ubyte * buffer, uint len, const NetAddress * address);
	int receive(ubyte * buffer, uint len);
	int receive(ubyte * buffer, uint len, NetAddress * address );

private:
	bool initializeTCP(const char * ipAddress, uint port, uint family = AF_INET);
	bool initializeUDP(const char * ipAddress, uint port, uint family = AF_INET);
	Socket accept();

	// receive packets and sort them as necessary.
	void receiveSort();

	static std::list<NetAddress> m_connectionAttempts;
	static std::list<NetAddress> m_disconnected;
	static std::list<Packet>     m_received;

	ProtoHeader * m_header;
	ProtoHeader m_lastProtoHeader;
	uint m_packetSize;
	bool m_isInitialized;
};