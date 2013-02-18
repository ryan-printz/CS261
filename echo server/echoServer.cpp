#include "Socket.h"
#include "TCPConnection.h"
#include <fstream>
#include <list>

int main(int argc, char *argv[])
{
	std::list<Socket*>		Listeners;
	std::list<IConnection*> Connections;
	std::fstream			Config("config.txt");

	initSockets();

	if( Config.is_open() )
		while(!Config.eof())
		{
			uint port;
			Config >> port;

			Socket * tcp = new Socket;
			tcp->initializeTCP(Socket::localIP(), port);
			tcp->setBlocking(false);
			tcp->bind();
			tcp->listen();

			Listeners.push_back(tcp);
		}
	else
		return 1;

	Connections.push_back(new TCPConnection());

	ubyte buffer[256];
	uint received;

	bool shutdown = false;
	while(!shutdown)
	{
		for(auto listener = Listeners.begin(); listener != Listeners.end(); ++listener)
			if( Connections.back()->accept((*listener)) )
				Connections.push_back(new TCPConnection());

		for(auto connection = Connections.begin(); connection != Connections.end(); ++connection)
			if( !(*connection)->connected() )
				continue;
			else if( 0 < (received = (*connection)->receive(buffer, 256)) )
				for(auto outConnection = Connections.begin(); outConnection != Connections.end(); ++outConnection)
					(*outConnection)->send(buffer, received);
			else if( /*would block*/ received )
				continue;
			else /* disconnected */
				delete *connection, connection = Connections.erase(connection);
	}

	for(auto listener = Listeners.begin(); listener != Listeners.end(); ++listener)
		delete * listener;

	for(auto connection = Connections.begin(); connection != Connections.end(); ++connection)
		delete * connection;

	cleanSockets();
	return 0;
}