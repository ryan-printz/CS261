#include "TCPConnection.h"
#include <iostream>

TCPConnection::TCPConnection()
{}

bool TCPConnection::connect(Socket * listener)
{
	Socket accepted = listener->accept();

	if( !accepted.invalid() )
		m_connection = accepted;

	return !accepted.invalid();
}

bool TCPConnection::connect(char * ip, uint port)
{
	if( !m_connection.initializeTCP(ip, port) )
		return false;

	if( !m_connection.connect() )
		return false;

	return true;
}

int TCPConnection::receive(ubyte * buffer, uint bufferlen)
{
	if( m_connection.invalid() ) 
		return -1;

	return m_connection.receive(buffer, bufferlen);
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
	{
		char buffer[8];
		_itoa_s<8>(m_connection.port(), buffer, 10);
		info.append(" ").append(m_connection.ipAddress()).append(":").append(buffer);
	}
	else
		info += " not connected.";

	return info;
}

void TCPConnection::close()
{
	m_connection.cleanUp();	
}
