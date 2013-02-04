#pragma once

#include "IConnection.h"
#include "Socket.h"

class ConnectionBase : public IConnection
{
protected:
	Socket m_connection;

public:
	ConnectionBase();
	~ConnectionBase();

	virtual int send(ubyte * buffer, uint bufferlen);
	virtual int recv(ubyte * buffer, uint bufferlen);
	virtual std::string connectionInfo() const;
};
