#include "Socket.h"
#include "ProtoConnection.h"

#include <iostream>

int main(void)
{
	if( !initSockets(true) )
		return 1;

	Socket udpsocket;
	udpsocket.initializeUDP(Socket::localIP(), 3000);
	udpsocket.bind();
	udpsocket.setBlocking(true);

	IConnection * udpconnection = new ProtoConnection();
	if( udpconnection->accept(&udpsocket) )
		std::cout << "client connected" << std::endl;

	char buffer[256];
	while( !udpconnection->receive((ubyte*)buffer, 256) );

	std::cout << buffer << std::endl;
	
	std::getchar();

	// cleanup.
	cleanSockets();
	return 0;
}