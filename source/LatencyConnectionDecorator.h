#pragma once

#include "IConnectionDecorator.h"
#include <sstream>
#include <list>

struct Delayed
{
	ubyte	m_packet[256];
	float	m_time;
	uint	m_size;
};

// delays all packets received by this connection by a specified amount.
class LatencyConnectionDecorator : public IConnectionDecorator
{
public:
	LatencyConnectionDecorator(float delay)
	{
		m_decorate = nullptr;
		m_delay	   = delay;
	}

	virtual ~LatencyConnectionDecorator() {}

	virtual void update(float dt)
	{
		for(auto packet = m_packets.begin(); packet != m_packets.end(); ++packet )
			packet->m_time += dt;

		m_decorate->update(dt);
	}

	virtual int receive(ubyte * buffer, uint len)
	{
		Delayed packet;
		packet.m_time = 0.0f;
		packet.m_size = m_decorate->receive( packet.m_packet, len );

		if( packet.m_size > 0 )
			m_packets.push_back( packet );

		if( m_packets.front().m_time >= m_delay )
		{
			memcpy( buffer, m_packets.front().m_packet, len );
			int size = packet.m_size;
			m_packets.pop_front();
			return size;
		}
		else 
			return -1;
	}

	virtual std::string connectionInfo() const
	{
		std::stringstream info;
		info << "Modified " << m_decorate->connectionInfo() << " delayed by " << m_delay;
		return info.str();
	};

private:
	std::list<Delayed> m_packets;
	float m_delay;
};
