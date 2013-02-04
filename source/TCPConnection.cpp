#include "TCPConnection.h"
#include <iostream>

TCPConnection::TCPConnection(Socket * listener)
	: m_listener(listener)
{}

bool TCPConnection::connect()
{
	Socket accepted = m_listener->accept();

	if( !accepted.invalid() )
		m_connection = accepted;

	return !accepted.invalid();
}

int TCPConnection::recieve(ubyte * buffer, uint bufferlen)
{
	if( m_connection.invalid() ) 
		return -1;

	return m_connection.recieve(buffer, bufferlen);
}

int TCPConnection::send(ubyte * buffer, uint bufferlen)
{
	if( m_connection.invalid() ) 
		return -1;

	return m_connection.send(buffer, bufferlen);
}

std::string TCPConnection::connectionInfo() const
{
	std::string info = "TCP Connection:";

	if( m_connection.connected() )
		info + " not connected.";
	else
	{
		char buffer[32];
		info + " " + m_connection.ipAddress() + ":" + itoa(m_connection.port(), buffer, 10);
	}
}

void TCPConnection::close()
{
	m_connection.cleanUp();	
}
