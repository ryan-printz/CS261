#pragma once

#include "IConnectionDecorator.h"

#include <sstream>

class DropConnectionDecorator : public IConnectionDecorator
{
public:
	DropConnectionDecorator(int high, int low = 0)
	{
		m_decorate = nullptr;
		
		// maintain sanity.
		if( high < low )	std::swap(high, low);
		if( high > 100 )	high = 100;
		if( low < 0 )		low = 0;

		m_highDrop = high;
		m_lowDrop = low;
	};

	virtual int receive(ubyte * buffer, uint len) 
	{
		if( (std::rand() % 100) + m_lowDrop > (100 - m_highDrop) )
			return -1;

		return m_decorate->receive(buffer, len);
	}

	virtual std::string connectionInfo() const
	{
		std::stringstream info;
		info << m_decorate->connectionInfo();
		info << "Drop Decorator" << std::endl << "-----------------------" << std::endl;
		info << "Drop ";

		if( m_lowDrop )
			info << m_lowDrop << "% - ";

		info << m_highDrop << "% of packets." << std::endl;
		return info.str();
	};

protected:
	int m_lowDrop;
	int m_highDrop;
};
