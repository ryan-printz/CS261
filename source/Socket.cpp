#include "Socket.h"
#include <iostream>

#pragma comment(lib, "WS2_32.lib");

bool initSockets(bool printInfo)
{
	static WSAData s_wsData;
	int result;

	result = WSAStartup(MAKEWORD(2,2), &s_wsData);

	if( result )
	{
		if( printInfo ) std::cout << "WinSock Startup Failed. #" << WSAGetLastError() << "!" << std::endl;
		return false;
	}

	if( printInfo ) std::cout << "WinSock 2.2 initialized!" << std::endl;

	return true;
}

bool cleanSockets()
{
	return WSACleanup() != SOCKET_ERROR;
}

Socket::Socket() 
	: m_isBlocking(true), m_isConnected(false), m_error(0), m_socket(INVALID_SOCKET)
{}

char * Socket::localIP()
{
	// get the ip for localhost.
	hostent * entity = gethostbyname("");

	return inet_ntoa( *(in_addr*)*entity->h_addr_list );
}

bool Socket::initializeTCP(char * ipAddress, uint port, uint family)
{
	m_address.sin_family = family;
	m_address.sin_port = htons(port);
	m_address.sin_addr.s_addr = inet_addr(ipAddress);

	m_socket = socket(family, SOCK_STREAM, IPPROTO_TCP);

	if( m_socket == INVALID_SOCKET )
	{
		m_error = WSAGetLastError();
		return false;
	}

	return true;
}

bool Socket::initializeUDP(char * ipAddress, uint port, uint family)
{
	m_address.sin_family = family;
	m_address.sin_port = htons(port);
	m_address.sin_addr.s_addr = inet_addr(ipAddress);

	m_socket = socket(family, SOCK_DGRAM, IPPROTO_UDP);

	if( m_socket == INVALID_SOCKET )
	{
		m_error = WSAGetLastError();
		return false;
	}

	return true;
}

bool Socket::cleanUp()
{
	int error = 0;

	if( m_isConnected )
	{
		error = shutdown(m_socket, SD_BOTH);
		if( error )
		{
			m_error = WSAGetLastError();
			return false;
		}
	}

	error = closesocket(m_socket);
	if( error )
	{
		m_error = WSAGetLastError();
		return false;
	}

	return true;
}

bool Socket::bind()
{
	// bind the socket.
	int error = ::bind(m_socket, (SOCKADDR*)&m_address, sizeof(m_address));

	if( error )
	{
		m_error = WSAGetLastError();
		return false;			
	}

	return true;
}

bool Socket::bind(const NetAddress * address)
{
	// bind the socket.
	int error = ::bind(m_socket, (SOCKADDR*)address, sizeof(address));

	if( error )
	{
		m_error = WSAGetLastError();
		return false;			
	}

	return true;
}

bool Socket::connect()
{
	// connect the socket to an address;
	int error = ::connect(m_socket, (SOCKADDR*)&m_address, sizeof(m_address));

	if( error )
	{
		m_error = WSAGetLastError();
		return false;
	}

	m_isConnected = true;
	return true;
}

bool Socket::connect(const NetAddress * address)
{
	// connect the socket to an address;
	int error = ::connect(m_socket, (SOCKADDR*)address, sizeof(address));

	if( error )
	{
		m_error = WSAGetLastError();
		return false;
	}

	m_isConnected = true;
	return true;
}

bool Socket::listen(ubyte backlog)
{
	int error = ::listen(m_socket, backlog);

	if( error )
	{
		m_error = WSAGetLastError();
		return false;
	}

	return true;
}

bool Socket::setBlocking(bool blocking)
{
	// socket is already setup correctly.
	if( m_isBlocking == blocking )
		return true;

	// enable/disable non-blocking mode.
	ulong val = (blocking)?0:1;
	int error = ioctlsocket(m_socket, FIONBIO, &val);

	if( error )
	{
		m_error = WSAGetLastError();
		return false;
	}

	m_isBlocking = blocking;
	return true;
}

bool Socket::invalid() const
{
	return m_socket == INVALID_SOCKET;
}

bool Socket::connected() const
{
	return m_isConnected;
}

char * Socket::ipAddress() const
{
	return inet_ntoa(m_address.sin_addr); 
}

uint Socket::port() const
{
	return ntohs(m_address.sin_port);
}

uint Socket::error() const
{
	return m_error;
}

int Socket::send(const ubyte * buffer, uint size)
{
	int sent = ::send(m_socket, (char*)buffer, size, 0);

	if( sent == SOCKET_ERROR )
		m_error = WSAGetLastError();

	return sent;
}

int Socket::receive(ubyte * buffer, uint size)
{
	int received = ::recv(m_socket, (char*)buffer, size, 0);
		
	// connection closed
	if( received == 0 )
	{
		m_isConnected = false;
		return 0;
	}
	else if( received == SOCKET_ERROR )
	{
		m_error = WSAGetLastError();
		return received;
	}

	return received;
}

Socket Socket::accept()
{
	Socket accepted;
	int addrlen = sizeof(accepted.m_address);

	// returns a new socket that the connection is actually made on.
	accepted.m_socket = ::accept(m_socket, (SOCKADDR*)&accepted.m_address, &addrlen);

	if( accepted.m_socket != INVALID_SOCKET )
		accepted.m_isConnected = true;
	else
		m_error = WSAGetLastError();

	return accepted;
}
