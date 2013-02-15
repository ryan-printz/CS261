#pragma once

#include "IConnection.h"
#include "Socket.h"

#include <unordered_map>

// this isn't really a connection, it just acts like one.
class ProtoConnection : public IConnection
{
public: 
	ProtoConnection();

	bool connect(char * ip, uint port);
	bool connect(Socket * open, char * ip, uint port);

	bool cleanUp();

	static int addProtoHeader(ubyte * message, uint size, uint priority, bool requestAck);

	virtual int receive(ubyte * buffer, uint len);
	virtual int send(ubyte * buffer, uint len);

	virtual int update(float dt);

	virtual std::string connectionInfo() const;

private:
	int makeAck(ubyte * buffer);

	bool m_connected;
	bool m_isClient;
	uint m_timeout;
	ubyte m_sequenceNumber;
	static const uint s_maxSize = 256;
	static const uint s_headerSize = 5;

	Socket * m_socket;
	// the other receiving endpoint.
	NetAddress m_connection;

	enum msgflags
	{
		MSG_REQUESTACK		= 0x01,
		MSG_ACK				= 0x02,
		MSG_RESEND			= 0x04,

		MSG_LOW_PRIORITY	= 0x10,
		MSG_HIGH_PRIORITY	= 0x20
	};

	struct msgHeader
	{
		ubyte	flags;
		ubyte	sequenceNumber;
		ushort	length;
	};

	struct msgInfo
	{
		ubyte	message[s_maxSize];
		ushort	length;
	};

	// messages that are awaiting ack.
	std::unordered_map<uint, msgInfo> m_resend;
	// messages that were missed.
	std::list<int> m_missedPackets;
};
