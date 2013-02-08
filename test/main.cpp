#include "TCPConnection.h"
#include "UDPConnection.h"
#include "DropConnectionDecorator.h"
#include "LatencyConnectionDecorator.h"

#include <iostream>

int main(void)
{
	if( !initSockets(true) )
		return 1;

	/// TCP
	Socket tcplistener;
	if(!tcplistener.initializeTCP(Socket::localIP(), 1024))
		std::cout << "error initializing tcp socket #" << tcplistener.error() << std::endl;
	else
		std::cout << "TCP Listener: " << tcplistener.localIP() << ":" << tcplistener.port() << std::endl;

	if(!tcplistener.bind())
		std::cout << "error binding tcp socket #" << tcplistener.error() << std::endl;

	if(!tcplistener.listen(10))
		std::cout << "error setting listen #" << tcplistener.error() << std::endl;

	IConnection * tcpConnection = new TCPConnection();
	std::cout << "waiting for a connection..." << std::endl;

	if(static_cast<TCPConnection*>(tcpConnection)->connect(&tcplistener))
		std::cout << "Client Connected!" << std::endl;
	else
		std::cout << "Error connecting! #" << WSAGetLastError() << std::endl;

	std::cout << std::endl << tcpConnection->connectionInfo() << std::endl;

	IConnection * modifiedTCPConnection = new DropConnectionDecorator(tcpConnection);

	std::cout << modifiedTCPConnection->connectionInfo() << std::endl;

	// wait for input.
	std::getchar();	
	
	static_cast<TCPConnection*>(tcpConnection)->close();

	if(!tcplistener.cleanUp())
		std::cout << "Error cleaning up tcplistener #:" << tcplistener.error() << std::endl;
	/// TCP

	/// UDP
	Socket udphost;
	if(!udphost.initializeUDP(Socket::localIP(), 1024))
		std::cout << "Error initializing UDP socket #" << udphost.error() << std::endl;
	else
		std::cout << "UDP host: " << udphost.localIP() << ":" << udphost.port() << std::endl;

	if(!udphost.bind())
		std::cout << "Error binding udphost. #" << udphost.error() << std::endl;

	udphost.setBlocking(true);

	IConnection * udpConnection = new UDPConnection();
	std::cout << "waiting for a connection..." << std::endl;

	if(!static_cast<UDPConnection*>(udpConnection)->connect(&udphost, Socket::localIP(), 1024))
		std::cout << "Error connecting..." << std::endl;

	char buffer[256];
	udpConnection->receive((ubyte*)buffer, 256);

	std::cout << "datagram: " << buffer << std::endl;

	std::cout << std::endl << udpConnection->connectionInfo() << std::endl;

	std::getchar();

	if(!udphost.cleanUp())
		std::cout << "Error cleaning up udphost #" << udphost.error() << std::endl;
	/// UDP

	// cleanup.
	cleanSockets();
	return 0;
}