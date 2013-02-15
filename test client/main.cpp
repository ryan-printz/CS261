#include "TCPConnection.h"
//#include "UDPConnection.h"
#include "DropConnectionDecorator.h"
#include "LatencyConnectionDecorator.h"

#include <iostream>

int main(void)
{
	/////TCP
	//if( !initSockets(true) )
	//	return 1;

	//IConnection * tcpConnection = new TCPConnection();

	//if(static_cast<TCPConnection*>(tcpConnection)->connect(Socket::localIP(), 1024))
	//	std::cout << "Connected!" << std::endl;
	//else
	//	std::cout << "Error connecting! #" << WSAGetLastError() << std::endl;

	//if( 14 != tcpConnection->send((ubyte*)"hello, world!", 14) )
	//	std::cout << "send failed!" << std::endl;

	//std::cout << std::endl << tcpConnection->connectionInfo() << std::endl;

	//// wait for input.
	//std::getchar();

	//static_cast<TCPConnection*>(tcpConnection)->close();
	/////TCP

	///// UDP
	//IConnection * udpConnection = new UDPConnection();

	//if(!static_cast<UDPConnection*>(udpConnection)->connect(Socket::localIP(), 1024))
	//	std::cout << "Error connecting! #" << WSAGetLastError() << std::endl;

	//if( 14 != udpConnection->send((ubyte*)"hello, world!", 14) )
	//	std::cout << "send failed!" << std::endl;

	//std::cout << std::endl << udpConnection->connectionInfo() << std::endl;

	//std::getchar();

	//if(!static_cast<UDPConnection*>(udpConnection)->cleanUp())
	//	std::cout << "Error closing connection! #" << WSAGetLastError() << std::endl;
	///// UDP

	//// cleanup.
	//cleanSockets();
	return 0;
}