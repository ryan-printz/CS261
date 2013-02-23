#pragma once

#include "IConnection.h"
#include "Socket.h"
#include "SequenceNumber.h"
#include "ProtoHeader.h"

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

struct ResendPacket
{
	SequenceNumber m_sequence;
	ubyte		   m_packet[256];
	float		   m_time;
	uint		   m_size;
};

class ResendOrderedQueue : public std::list<ResendPacket>
{
public:
	bool has(SequenceNumber no) const
	{
		for(const_iterator packet = begin(); packet != end(); ++packet)
			if( packet->m_sequence == no )
				return true;
		return false;
	}

	void advance(float dt)
	{
		for(iterator packet = begin(); packet != end(); ++packet)
			packet->m_time += dt;
	}

	void insert(const ResendPacket & rp)
	{
		if( empty() )
		{
			push_back(rp);
			return;
		}

		if( rp.m_sequence <= front().m_sequence )
			push_front(rp);
		else if( rp.m_sequence > back().m_sequence )
			push_back(rp);
		else
			for(iterator ipoint = begin(); ipoint != end(); ++ipoint)
				if( rp.m_sequence < ipoint->m_sequence )
				{
					std::list<ResendPacket>::insert(ipoint, rp);
					break;
				}
	}
};

struct FlowPacket
{
	ubyte	m_buffer[256];
	ubyte	m_flags;
	uint	m_size;
};

bool operator==(const FlowPacket &lhs, const FlowPacket &rhs);
bool operator!=(const FlowPacket &lhs, const FlowPacket &rhs);
bool operator >(const FlowPacket &lhs, const FlowPacket &rhs);
bool operator <(const FlowPacket &lhs, const FlowPacket &rhs);
bool operator>=(const FlowPacket &lhs, const FlowPacket &rhs);
bool operator<=(const FlowPacket &lhs, const FlowPacket &rhs);

class LinkedPriorityQueue : public std::list<FlowPacket>
{
public:
	void insert(const FlowPacket & fp)
	{
		if( empty() )
			push_back(fp);

		if( fp <= front() )
			push_front(fp);
		else if( fp >= back() )
			push_back(fp);
		else
			for(iterator iplace = begin(); iplace != end(); ++iplace)
				if( fp < *iplace )
					std::list<FlowPacket>::insert(iplace, fp);
	};
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

std::ostream & operator<<(std::ostream & os, const ConnectionStats & stats);

// this isn't really a connection, it just acts like one.
class ProtoConnection : public IConnection
{
public: 
	ProtoConnection();

	virtual bool accept(Socket * open);
	virtual bool connect(const char * ip, uint port);
	virtual bool cleanup();
	virtual bool disconnect();

	virtual bool connected() const;

	virtual int receive(ubyte * buffer, uint len, int drop = 0);
	virtual int send(ubyte * buffer, uint len, ubyte flags = ProtoHeader::PROTO_NORMAL);
	virtual void update(float dt);

	virtual std::string connectionInfo() const;

	const ConnectionStats& getConnectionStats() const; 

protected:
	void updateStats();
	void updateRTT(float time);

	uint makeAck();
	void useAck(SequenceNumber ack, uint ackPack, bool resent);
	ubyte getBitIndex(const SequenceNumber & lhs, const SequenceNumber & rhs);
	int noFlowSend(ubyte * buffer, uint len, ubyte flags);

	void updateFlowControl(float dt);

private:
	// the other receiving endpoint.
	ProtoSocket m_connection;

	// connection properties
	bool m_connected;
	bool m_server;
	uint m_timeout;
	uint m_goodRoundTripTime;
	uint m_keepAliveInterval;

	float m_idleTimer;

	bool m_useFlowControl;
	float m_flowTimer;
	float m_sendRate;
	float m_modeTimer;
	float m_penaltyTimer;
	float m_unpenaltyTimer;

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

	ResendOrderedQueue m_resend;
	LinkedPriorityQueue m_flowControl;
};
