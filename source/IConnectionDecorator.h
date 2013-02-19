#pragma once

#include "IConnection.h"

class IConnectionDecorator : public IConnection
{
public:
	virtual ~IConnectionDecorator();

// IConnectionDecorator functions
	void decorate(IConnection ** connection);
	static void undecorate(IConnection ** connection);

	// delete the decorated connection;
	void destroy();

// members inherited from IConnection
	virtual bool accept(Socket * open);
	virtual bool connect(char * ip, uint port);
	virtual bool cleanup();
	virtual bool disconnect();
	virtual bool connected();
	virtual int receive(ubyte * buffer, uint len);
	virtual int send(ubyte * buffer, uint len, ubyte flags = 0);
	virtual void update(float dt);
	virtual std::string connectionInfo() const;

protected:
	IConnection * m_decorate;
};
