#include "UDPConnection.h"

UDPConnection::UDPConnection()
	: m_socket(nullptr), m_isClient(false)
{}

// socket and the destination ip:port pair.
// the socket should be bound on the server end.
bool UDPConnection::connect(Socket * socket, char * IP, uint port)
{
	m_socket = socket;

	m_connection.sin_family = AF_INET;
	m_connection.sin_port = htons(port);
	m_connection.sin_addr.s_addr = inet_addr(IP);

	return true;
}

bool UDPConnection::connect(char * IP, uint port)
{
	m_socket = new Socket();

	m_socket->initializeUDP(Socket::localIP(), port, AF_INET);

	m_connection.sin_family = AF_INET;
	m_connection.sin_port = htons(port);
	m_connection.sin_addr.s_addr = inet_addr(IP);

	m_isClient = true;

	return true;
}

bool UDPConnection::cleanUp()
{
	if( m_isClient ) 
	{
		m_socket->cleanUp();
		delete m_socket;
	}

	return true;
}

// send a packet to the "connected" 
int UDPConnection::send(ubyte * buffer, uint len)
{
	return m_socket->send(buffer, len, &m_connection);
}

// receives a datagram.
// there is no logic for the connection, 
// receives all data sent to the port, regardless of 
// the sender.
int UDPConnection::receive(ubyte * buffer, uint len)
{
	return m_socket->receive(buffer, len, nullptr);
}

std::string UDPConnection::connectionInfo() const
{
	std::string info = "UDP Connection:";

	if( !m_socket )
	{
		char buffer[16];
		_itoa_s<16>(ntohs(m_connection.sin_port), buffer, 10);
		info.append(" ").append(inet_ntoa(m_connection.sin_addr)).append(":").append(buffer);
	}
	else
		info + " not connected.";

	return info;
}
