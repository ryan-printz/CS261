#pragma once

#include "IConnection.h"
#include "IDecorator.h"

class DropConnectionDecorator : public IConnection, public IDecorator
{
public:
	DropConnectionDecorator(IConnection * modifiedConnection)
		: m_decorated(modifiedConnection)
	{};

	virtual int send(ubyte * buffer, uint bufferlen) { return m_decorated->send(buffer, bufferlen); };
	virtual int receive(ubyte * buffer, uint bufferlen) { return m_decorated->receive(buffer, bufferlen); };

	virtual std::string connectionInfo() const
	{
		return "Modified " + m_decorated->connectionInfo() + " (% of packets dropped.)";
	};

private:
	IConnection * m_decorated;
};
