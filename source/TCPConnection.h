#include "IConnection.h"
#include "Socket.h"

class TCPConnection : public IConnection
{
public:
	TCPConnection();

	// takes a reference to a Socket 
	// in listening mode.
	bool connect(Socket * listener);
	bool connect(char * ip, uint port);

	void close();

	virtual int send(ubyte * buffer, uint bufferlen);
	virtual int receive(ubyte * buffer, uint bufferlen);
	virtual std::string connectionInfo() const;

protected:
	Socket m_connection;
};
