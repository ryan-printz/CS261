#pragma once

#include "IConnection.h"
#include "Socket.h"

class TCPConnection : public IConnection
{
public:
	TCPConnection();

	virtual bool accept(Socket * open);
	virtual bool connect(char * ip, uint port);
	virtual bool cleanup();
	virtual bool disconnect();

	virtual bool connected() const;

	virtual int send(ubyte * buffer, uint bufferlen);
	virtual int receive(ubyte * buffer, uint bufferlen);

	virtual void update(float dt);

	virtual std::string connectionInfo() const;

protected:
	Socket m_connection;
};
