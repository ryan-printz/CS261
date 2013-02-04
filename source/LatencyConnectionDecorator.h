#pragma once

#include "IConnection"
#include "IDecorator"

class LatencyConnectionDecorator : public IConnection, public Decorator
{
public:
	LatencyConnectionDecorator(IConnection * modifiedConnection)
	{};

	virtual int send(ubyte * buffer, uint bufferlen) {};
	virtual int recv(ubyte * buffer, uint bufferlen) {};
	virtual std::string connectionInfo() const
	{
		return "Modified " + m_decorated.connectionInfo() + " delayed by ";
	};

private:
	IConnection * m_decorated;
};
