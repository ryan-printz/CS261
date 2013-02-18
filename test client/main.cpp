#include "Socket.h"
#include "ProtoConnection.h"

#include <iostream>

int main(void)
{
    if( !initSockets(true) )
        return 1;

	IConnection * udpconnection = new ProtoConnection();

	udpconnection->connect(Socket::localIP(), 3000);

	const char buffer[] = "Message Fucking Received.";
	udpconnection->send((ubyte*)buffer, sizeof(buffer));

	std::getchar();

    // cleanup.
    cleanSockets();
    return 0;
}