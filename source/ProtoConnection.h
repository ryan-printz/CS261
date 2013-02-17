#pragma once

#include "IConnection.h"
#include "Socket.h"
#include "SequenceNumber.h"

#include <unordered_map>

#include <list>

// data about sent and received packets.
// used for calculating stats.
struct PacketInfo
{
	SequenceNumber m_sequence;
	float		   m_time;
	uint		   m_size;
};

class LinkedOrderedQueue : public std::list<PacketInfo>
{
public:
	bool has(SequenceNumber no) const
	{
		for(const_iterator info = begin(); info != end(); ++info)
			if( info->m_sequence == no )
				return true;
		return false;
	}

	void advance(float dt)
	{
		for(iterator info = begin(); info != end(); ++info)
			info->m_time += dt;
	}

	void insert(const PacketInfo & pi)
	{
		if( empty() )
		{
			push_back(pi);
			return;
		}

		if( pi.m_sequence <= front().m_sequence )
			push_front(pi);
		else if( pi.m_sequence > back().m_sequence )
			push_back(pi);
		else
			for(iterator ipoint = begin(); ipoint != end(); ++ipoint)
				if( pi.m_sequence < ipoint->m_sequence )
				{
					std::list<PacketInfo>::insert(ipoint, pi);
					break;
				}
	}
};

struct ConnectionStats
{
	float m_upBandwidth;
	float m_downBandwith;
	float m_roundTripTime;

	int m_sentPackets;
	int m_receivedPackets;
	int m_lostPackets;
	int m_ackedPackets;
};

std::ostream & operator<<(std::ostream & os, const ConnectionStats & stats)
{
	std::ostream::sentry ok(os);
	os << stats.m_upBandwidth << "/" << stats.m_downBandwith << "up/down. ping: " << stats.m_roundTripTime << std::endl; 
	os << "lifetime sent:" << stats.m_sentPackets << " acked: " << stats.m_ackedPackets;
	os << " received: " << stats.m_receivedPackets << " lost: " << stats.m_lostPackets;
	return os;
}

// this isn't really a connection, it just acts like one.
class ProtoConnection : public IConnection
{
public: 
	struct ProtoHeader
	{
		// a constant identifier for the protocol.
		// packets that don't have this are ignored.
		ProtoHeader() : m_protocol(0xb4639620) {}
		const uint		m_protocol;

		SequenceNumber	m_sequence;
		SequenceNumber	m_ack;
		uint			m_acks;

		bool valid() { return m_protocol == 0xb4639620; };
	};

	ProtoConnection();
	bool connect(char * ip, uint port);
	bool connect(Socket * open, char * ip, uint port);
	bool cleanUp();
	virtual int receive(ubyte * buffer, uint len);
	virtual int send(ubyte * buffer, uint len);
	virtual void update(float dt);
	virtual std::string connectionInfo() const;

private:
	void updateStats();
	void updateRTT(float time);

	uint makeAck();
	void useAck(SequenceNumber ack, uint ackPack);
	ubyte getBitIndex(const SequenceNumber & lhs, const SequenceNumber & rhs);

	bool m_connected;
	bool m_isClient;
	
	// the other receiving endpoint.
	Socket * m_socket;
	NetAddress m_connection;

	float m_idleTimer;

	// connection properties
	uint m_timeout;
	uint m_keepAliveInterval;

	// connection stats
	ConnectionStats m_stats;

	// sequence numbers;
	SequenceNumber m_local;
	SequenceNumber m_remote;

	// messages that were acked last update.
	std::list<uint> m_acks;

	// sent and received messages.
	LinkedOrderedQueue m_unackedPackets;
	LinkedOrderedQueue m_ackedPackets;

	// used for calculating sent bandwith per second
	LinkedOrderedQueue m_sentPackets;
	// used for determining which packets to ack.
	LinkedOrderedQueue m_receivedPackets;
};
