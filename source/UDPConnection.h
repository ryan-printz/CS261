#include "IConnection.h"
#include "Socket.h"

// this isn't really a connection, it just acts like one.
class UDPConnection : public IConnection
{
public: 
	UDPConnection();

	bool connect(char * ip, uint port);
	bool connect(Socket * open, char * ip, uint port);

	bool cleanUp();

	virtual int receive(ubyte * buffer, uint len);
	virtual int send(ubyte * buffer, uint len);

	virtual std::string connectionInfo() const;

private:
	bool m_isClient;
	Socket * m_socket;
	// the other receiving endpoint.
	NetAddress m_connection;
};
