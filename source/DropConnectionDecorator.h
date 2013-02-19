#pragma once

#include "IConnectionDecorator.h"

#include <sstream>

class DropConnectionDecorator : public IConnectionDecorator
{
public:
	DropConnectionDecorator(int droprate)
	{
		m_decorate = nullptr;
		m_droprate = droprate;
	};

	virtual int receive(ubyte * buffer, uint len) 
	{
		if( (std::rand() % 100 <= m_droprate) )
			return -1;

		return m_decorate->receive(buffer, len);
	}

	virtual std::string connectionInfo() const
	{
		std::stringstream info;
		info << "Modified " << m_decorate->connectionInfo() << " (" << 1.0f / m_droprate << "% of packets dropped.)";
		return info.str();
	};

protected:
	int m_droprate;
};
