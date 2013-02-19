#include "IConnectionDecorator.h"

IConnectionDecorator::~IConnectionDecorator()
{
	if(m_decorate)	delete m_decorate;
}

// attaches this decorator to the specified connection.
void IConnectionDecorator::decorate(IConnection ** connection)
{
	m_decorate = *connection;
	*connection = this;
}

void IConnectionDecorator::destroy()
{
	if(m_decorate) delete m_decorate;
}

// detaches the decorator
void IConnectionDecorator::undecorate(IConnection ** connection)
{
	IConnectionDecorator * decorator = dynamic_cast<IConnectionDecorator*>(*connection);

	if( decorator )
		*connection = decorator->m_decorate;
}

bool IConnectionDecorator::accept(Socket * open) 
{ 
	return m_decorate->accept(open); 
}

bool IConnectionDecorator::connect(char * ip, uint port)
{
	return m_decorate->connect(ip, port);
}

bool IConnectionDecorator::cleanup()
{
	return m_decorate->cleanup();
}

bool IConnectionDecorator::disconnect()
{
	return m_decorate->disconnect();
}

bool IConnectionDecorator::connected() const
{
	return m_decorate->connected();
}

int IConnectionDecorator::receive(ubyte * buffer, uint len)
{
	return m_decorate->receive(buffer, len);
}

int IConnectionDecorator::send(ubyte * buffer, uint len, ubyte flags)
{
	return m_decorate->send(buffer, len, flags);
}

void IConnectionDecorator::update(float dt)
{
	return m_decorate->update(dt);
}

std::string IConnectionDecorator::connectionInfo() const
{
	return m_decorate->connectionInfo();
}
