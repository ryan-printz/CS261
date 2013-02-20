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
	LatencyConnectionDecorator(int high, int low = 0)
	{
		m_decorate = nullptr;
		
		// maintain sanity.
		if( high < low )	std::swap(high, low);
		if( high > 1000 )	high = 1000;
		if( low < 0 )		low = 0;

		m_highDelay = high;
		m_lowDelay = low;
	}

	virtual ~LatencyConnectionDecorator() {}

	virtual void update(float dt)
	{
		for(auto packet = m_packets.begin(); packet != m_packets.end(); ++packet )
			packet->m_time -= dt;

		m_decorate->update(dt);
	}

	virtual int receive(ubyte * buffer, uint len)
	{
		Delayed packet;
		packet.m_time = float((std::rand() % (m_highDelay-m_lowDelay)) + m_lowDelay) / 1000.f;
		packet.m_size = m_decorate->receive( packet.m_packet, len );

		if( (int)packet.m_size > 0 )
			m_packets.push_back( packet );


		if( !m_packets.empty() )
		{
			if( m_packets.front().m_time <= 0.0f )
			{
				memcpy( buffer, m_packets.front().m_packet, len );
				int size = m_packets.front().m_size;
				m_packets.pop_front();
				return size;
			}
			else 
				return -1;
		}
		else
			return 0;
	}

	virtual std::string connectionInfo() const
	{
		std::stringstream info;
		info << m_decorate->connectionInfo();
		info << "Delay Decorator" << std::endl << "-----------------------" << std::endl;
		info << "Delay packets by";

		if( m_lowDelay )
			info << m_lowDelay << "ms - ";

		info << m_highDelay << "ms." << std::endl;
		return info.str();
	};

private:
	std::list<Delayed> m_packets;
	int m_lowDelay;
	int m_highDelay;
};
