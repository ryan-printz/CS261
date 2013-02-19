#include "DecoratorConnectionManager.h"

DecoratorConnectionManager::~DecoratorConnectionManager()
{
	// destroys decorated nodes only.
	// the decorators themselves will be destroyed by the 
	// container they're in.
	for( auto decorator = m_decorators.rbegin(); decorator != m_decorators.rend(); ++decorator)
		(*decorator)->destroy();
}

void DecoratorConnectionManager::Decorate(HSession session, IConnectionDecorator * decorator)
{
	auto connection = m_connections.find(session);
	if( connection == m_connections.end() )
		return;

	decorator->decorate( &connection->second );
	m_decorators.push_back(decorator);
}

void DecoratorConnectionManager::Undecorate(HSession session)
{
	auto connection = m_connections.find(session);
	if( connection == m_connections.end() )
		return;

	m_decorators.remove((IConnectionDecorator*)connection->second);
	IConnectionDecorator::undecorate( &connection->second );
}