#pragma once

#include "Types.h"
#include <WinSock2.h>

typedef sockaddr_in NetAddress;

bool initSockets(bool printInfo = false);
bool cleanSockets();

class Socket
{
public:
	Socket();
	virtual ~Socket();

	static char * localIP();

	bool initializeTCP(const char * ipAddress, uint port, uint family = AF_INET);
	bool initializeUDP(const char * ipAddress, uint port, uint family = AF_INET);
	bool cleanUp();

	bool bind();
	bool bind(const NetAddress * address);
	bool connect();
	bool connect(const NetAddress * address);
	bool listen(ubyte backlog = 10);
	bool setBlocking(bool blocking);

	bool connected() const;
	bool invalid() const;
	char * ipAddress() const;
	uint port() const;
	uint error() const;

	int send(const ubyte * buffer, uint size, const NetAddress * address);
	int send(const ubyte * buffer, uint size);
	int receive(ubyte * buffer, uint size, NetAddress * address);
	int receive(ubyte * buffer, uint size);

	Socket accept();

protected:
	uint m_error;
	bool m_isBlocking;
	bool m_isConnected;

	NetAddress	m_address;
	SOCKET		m_socket;
};
