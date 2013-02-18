#pragma once

#include "Types.h"
#include <string>

class Socket;

class IConnection
{
public:
	// connection setup/teardown
	// accept requires a fully initialized "listening" socket.
	virtual bool accept(Socket * open) = 0;
	virtual bool connect(char * ip, uint port) = 0;
	virtual bool cleanup() = 0;
	virtual bool disconnect() = 0;

	// send/receive
	virtual int receive(ubyte * buffer, uint len) = 0;
	virtual int send(ubyte * buffer, uint len) = 0;

	// update connection
	virtual void update(float dt) = 0;

	// print info
	virtual std::string connectionInfo() const = 0;
};

std::ostream& operator<<(std::ostream & os, const IConnection& connection);
