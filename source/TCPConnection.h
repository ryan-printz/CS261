#include "ConnectionBase.h"

class TCPConnection : public ConnectionBase
{
protected:
	SOCKET		m_client;
	sockaddr_in	m_clientAddress;

public:
	TCPConnection(char * localIP, hostent * localhost, uint port);

	// blocks until connect.
	void connect();
	void close();

	virtual int send(ubyte * buffer, int bufferlen);
	virtual int recv(ubyte * buffer, int bufferlen);
	virtual std::string connectionInfo() const;
};
