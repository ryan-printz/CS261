#pragma once

#include "IConnection.h"
#include "winsock2.h"

class ConnectionBase : public IConnection
{
protected:
	struct WinSockData 
	{
		WinSockData();
		~WinSockData();

		WSAData m_wsData;
	};

	static WinSockData s_winSock;

	SOCKET		m_socket;
	sockaddr_in m_socketAddress;

public:
	ConnectionBase();
	~ConnectionBase();

	virtual int send(ubyte * buffer, int bufferlen);
	virtual int recv(ubyte * buffer, int bufferlen);
	virtual std::string connectionInfo() const;
};
