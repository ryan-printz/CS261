#include "IConnection.h"
#include "Socket.h"

class TCPConnection : public IConnection
{
public:
	// Constructor. takes a reference to a Socket 
	// in listening mode.
	TCPConnection(Socket * m_listener);

	bool connect();
	void close();

	virtual int send(ubyte * buffer, uint bufferlen);
	virtual int recieve(ubyte * buffer, uint bufferlen);
	virtual std::string connectionInfo() const;

protected:
	Socket * m_listener;
	Socket m_connection;
};
