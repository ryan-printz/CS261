#include "TCPConnection.h"
#include <iostream>
#include <sstream>

TCPConnection::TCPConnection()
{}

bool TCPConnection::accept(Socket * listener)
{
	Socket accepted = listener->accept();

	if( !accepted.invalid() ) {
		m_connection = accepted;
        m_connection.setBlocking(false);
    }

	return !accepted.invalid();
}

bool TCPConnection::connect(const char * ip, uint port)
{
	if( !m_connection.initializeTCP(ip, port) )
		return false;

	if( !m_connection.connect() )
		return false;

    if (!m_connection.setBlocking(false))
        return false;

	return true;
}

void TCPConnection::update(float dt)
{}

bool TCPConnection::connected() const
{
	return m_connection.connected();
}

int TCPConnection::receive(ubyte * buffer, uint bufferlen, int drop)
{
	if( m_connection.invalid() ) 
		return -1;

	int temp = m_connection.receive(buffer, bufferlen);
	//Ignores the TCP packet if drop decorator is on. For testing
	if( drop == -1 )
		return drop;
	else
		return temp;
}

int TCPConnection::send(ubyte * buffer, uint bufferlen, ubyte)
{
	if( m_connection.invalid() ) 
		return -1;

	return m_connection.send(buffer, bufferlen);
}

std::string TCPConnection::connectionInfo() const
{
	std::stringstream info;
	
	info << "TCP Connection: ";

	if( m_connection.connected() )
		info << m_connection.ipAddress() << ":" << m_connection.port();
	else
		info << "not connected.";

	info << std::endl << "-----------------------" << std::endl;

	return info.str();
}

bool TCPConnection::disconnect()
{
	return true;
}

bool TCPConnection::cleanup()
{
	return m_connection.cleanUp();	
}
