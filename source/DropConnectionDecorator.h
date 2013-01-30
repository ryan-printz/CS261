#pragma once

#include "IConnection"
#include "IDecorator"

class DropConnectionDecorator : public IConnection, public Decorator
{
public:
	DropConnectionDecorator(IConnection * modifiedConnection)
	{};

	virtual int send(ubyte * buffer, int bufferlen) {};
	virtual int recv(ubyte * buffer, int bufferlen) {};
	virtual std::string connectionInfo() const
	{
		return "Modified " + m_decorated.connectionInfo() + "% of packets dropped.";
	};

private:
	IConnection * m_decorated;
};
