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

	static char * localIP();

	bool initializeTCP(char * ipAddress, uint port, uint family = AF_INET);
	bool initializeUDP(char * ipAddress, uint port, uint family = AF_INET);
	bool cleanUp();

	bool bind(const NetAddress * address);
	bool connect(const NetAddress * address);
	bool listen(ubyte backlog = 10);
	bool setBlocking(bool blocking);

	bool connected() const;
	bool invalid() const;
	char * ipAddress() const;
	uint port() const;

	int send(const ubyte * buffer, uint size);
	int recieve(ubyte * buffer, uint size);

	Socket accept();

private:
	uint m_error;
	bool m_isBlocking;
	bool m_isConnected;

	NetAddress	m_address;
	SOCKET		m_socket;
};
