#include "TCPConnection.h"
#include "DropConnectionDecorator.h"
#include "LatencyConnectionDecorator.h"

#include <iostream>

int main(void)
{
	if( !initSockets(true) )
		return 1;

	IConnection * tcpConnection = new TCPConnection();

	if(static_cast<TCPConnection*>(tcpConnection)->connect(Socket::localIP(), 1024))
		std::cout << "Connected!" << std::endl;
	else
		std::cout << "Error connecting! #" << WSAGetLastError() << std::endl;

	if( 14 != tcpConnection->send((ubyte*)"hello, world!", 14) )
		std::cout << "send failed!" << std::endl;

	std::cout << std::endl << tcpConnection->connectionInfo() << std::endl;

	// wait for input.
	std::getchar();

	static_cast<TCPConnection*>(tcpConnection)->close();

	// cleanup.
	cleanSockets();
	return 0;
}