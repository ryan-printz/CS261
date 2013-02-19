/*
*       ConnectionManager.cpp
*
*       by Tim Rowsey
*
*       2013/02/09   
*/
#include <cassert>
#include "IConnection.h"
#include "ConnectionManager.h"

#include <sstream>

//******************************************************************************
//ConnectionManager::ConnectionManager () {

//}

//******************************************************************************
ConnectionManager::~ConnectionManager () {

    ClearDeleteList();
}

unsigned ConnectionManager::Count() const
{
	return m_connections.size();
}

HSession ConnectionManager::GetHandleByIndex(uint sessionNumber) const
{
	auto connection = m_connections.begin();

	while( --sessionNumber > 0 )
		++connection;

	if( connection == m_connections.end() )
		return nullptr;
	else
		return connection->first;
}

std::string ConnectionManager::GetAllSessionInfo() const
{
	std::stringstream info;

	info << "Current Sessions: " << Count() << std::endl << "--------------------------------------------------" << std::endl;

	int i = 1;
	for(auto connection = m_connections.begin(); connection != m_connections.end(); ++connection)
		info << i++ << ": " << connection->second->connected() << std::endl;

	return info.str();
}

//******************************************************************************
std::string ConnectionManager::GetSessionInfo (HSession session) const {
	auto connection = m_connections.find(session);

	if( connection == m_connections.end() )
		return "Invalid Session.";

	return connection->second->connectionInfo();
}

//******************************************************************************
void ConnectionManager::Add (IConnection * connection) {
    assert(connection != nullptr);

    m_connections.emplace(std::make_pair((HSession)connection, connection));
}

//******************************************************************************
void ConnectionManager::Remove (IConnection * connection) {
    assert(connection != nullptr);

    m_deleteList.insert(connection);
}

//******************************************************************************
void ConnectionManager::ClearDeleteList () {
    auto delItr = m_deleteList.begin();

    for (; delItr != m_deleteList.end(); ++delItr) 
	{
		auto connection = m_connections.find((HSession)*delItr);

		if( connection == m_connections.end() )
			continue;

		delete connection->second;
		m_connections.erase(connection);
    }

    m_deleteList.clear();
}

//******************************************************************************
bool ConnectionManager::IsHandleValid (HSession session) const {
	return m_connections.find(session) != m_connections.end();
}

//******************************************************************************
void ConnectionManager::Send (unsigned char * buffer, unsigned bufferLen, HSession session) {
	auto connection = m_connections.find(session);

	if( connection == m_connections.end() )
		return;

	connection->second->send(buffer, bufferLen);
}

//******************************************************************************
int ConnectionManager::Receive (unsigned char * buffer, unsigned bufferLen, HSession& sessionOut) {
    auto connectionItr = m_connections.begin();
    int result = 0;
    sessionOut = nullptr;

    for (; connectionItr != m_connections.end(); ++connectionItr) {
		result = connectionItr->second->receive(buffer, bufferLen);

        if (result != 0 && result != -1)
			sessionOut = connectionItr->first;
            break;
    }

    return result;
}
//******************************************************************************
void ConnectionManager::Update (float dt) {
    auto connectionItr = m_connections.begin();

    for (; connectionItr != m_connections.end(); ++connectionItr) {
        connectionItr->second->update(dt);
    }

}

//******************************************************************************
void ConnectionManager::Broadcast (unsigned char * buffer, unsigned bufferLen) {
    auto connectionItr = m_connections.begin();

    for (; connectionItr != m_connections.end(); ++connectionItr) {
		connectionItr->second->send(buffer, bufferLen);
    }
}
