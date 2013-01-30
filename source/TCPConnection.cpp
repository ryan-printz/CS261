#include "TCPConnection.h"
#include <iostream>

TCPConnection::TCPConnection(char * localIP, hostent * localhost, uint port)
{
	// local endpoint
	m_socketAddress.sin_family = AF_INET;
	m_socketAddress.sin_port = htons(port);
	m_socketAddress.sin_addr.s_addr = inet_addr(localIP);

	// create the socket
	m_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);

	if(INVALID_SOCKET == m_socket)
	{
		std::cout << "Socket call failed! " << WSAGetLastError() << std::endl;
		WSACleanup();
		return;
	}

	int result = bind(m_socket, (SOCKADDR*)&m_socketAddress, sizeof(m_socketAddress));
	if(SOCKET_ERROR == result)
		std::cout << "Bind call failed! " << WSAGetLastError() << std::endl;
}

void TCPConnection::connect()
{
	int result = listen(m_socket, 10);
	if(SOCKET_ERROR == result)
		std::cout << "Listen call failed! " << WSAGetLastError() << std::endl;

	int endpointSize = sizeof(m_clientAddress);
	m_client = accept(m_socket, (SOCKADDR*)&m_clientAddress, &endpointSize);
	if(SOCKET_ERROR == result)
		std::cout << "Accept call failed! " << WSAGetLastError() << std::endl;
}

int TCPConnection::recv(ubyte * buffer, int bufferlen)
{
	int recieved = ::recv(m_client, (char*)buffer, bufferlen, 0);
	if(SOCKET_ERROR == recieved)
	{
		std::cout << "Socket Error! " << WSAGetLastError() << std::endl;
		return -1;
	}

	return recieved;
}

int TCPConnection::send(ubyte * buffer, int bufferlen)
{
	return 0;
}

std::string TCPConnection::connectionInfo() const
{
	return ConnectionBase::connectionInfo() + " TCP connection:" + " IP Address:";
}


void TCPConnection::close()
{
	shutdown(m_client, SD_BOTH);
	shutdown(m_socket, SD_BOTH);

	closesocket(m_client);
	closesocket(m_socket);
}
