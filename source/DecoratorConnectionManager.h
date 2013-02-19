#pragma once

#include "ConnectionManager.h"
#include "IConnectionDecorator.h"

class DecoratorConnectionManager : public ConnectionManager
{
public:
	DecoratorConnectionManager() {}
	~DecoratorConnectionManager();

	// removes the topmost decorator from a session.
	// does nothing if the connection is not decorated.
	void Undecorate(HSession session);
	// decorate the specified session.
	void Decorate(HSession session, IConnectionDecorator * decorator);

protected:
	std::list<IConnectionDecorator*> m_decorators;
};
