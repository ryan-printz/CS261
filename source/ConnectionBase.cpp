#include "ConnectionBase.h"
#include <iostream>

#pragma comment(lib, "ws2_32.lib")

struct ConnectionBase::WinSockData ConnectionBase::s_winSock = WinSockData();

// initialize winsock.
ConnectionBase::WinSockData::WinSockData()
{
	SecureZeroMemory(&m_wsData, sizeof(m_wsData));
	int ret = WSAStartup(MAKEWORD(2,2), &m_wsData);

	if( ret != 0 ) std::cout << "Error initializing winsock! " << ret << std::endl;
}

// cleanup winsock
ConnectionBase::WinSockData::~WinSockData()
{
	WSACleanup();
}

ConnectionBase::ConnectionBase()
{}

ConnectionBase::~ConnectionBase()
{}

int ConnectionBase::send(ubyte * buffer, int bufferlen)
{
	return 0;
}

int ConnectionBase::recv(ubyte * buffer, int bufferlen)
{
	return 0;
}

std::string ConnectionBase::connectionInfo() const
{
	return std::string("WinSock2");
}
