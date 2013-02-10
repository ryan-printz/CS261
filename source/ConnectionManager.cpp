/*
*       ConnectionManager.cpp
*
*       by Tim Rowsey
*
*       2013/02/09   
*/

#include "ConnectionManager.hpp"
#include "IConnection.h"

//******************************************************************************
ConnectionManager::ConnectionManager () {

}

//******************************************************************************
ConnectionManager::~ConnectionManager () {
    ClearDeleteList();

    auto deleteItr = m_connections.begin();
    
    for (; deleteItr != m_connections.end(); ++deleteItr) {
        delete &(deleteItr->first);
        //deleteItr->second->close();
        delete deleteItr->second;
    }
}

//******************************************************************************
Session * const ConnectionManager::AddConnection (IConnection * connection) {

    // utilize temp variable to avoid unnecessary allocations
    Session * newSession, temp;
    newSession = &temp;

    newSession->connectionInfo = connection->connectionInfo();

    auto foundItr = m_connections.find(*newSession);

    if (foundItr == m_connections.end()) {
       newSession = new Session;
       m_connections[*newSession] = connection;
    }
    else {
        newSession = &(foundItr->first);
    }
        
    return newSession;
}

//******************************************************************************
IConnection * const ConnectionManager::FindConnection (Session * session) const {
    auto findItr = m_connections.find(*session);

    return (findItr != m_connections.end() ? findItr->second : NULL);
}

//******************************************************************************
void ConnectionManager::CloseConnection (Session ** session) {
    Session * toDelete = *session;
    *session = NULL;

    if (m_connections.find(*toDelete) != m_connections.end()) {
        m_connectionCloseSet.insert(toDelete);
    }
}

//******************************************************************************
void ConnectionManager::ClearDeleteList () {
    auto closeSetItr = m_connectionCloseSet.begin();

    Session * temp;
    for (; closeSetItr != m_connectionCloseSet.end(); ++closeSetItr) {
        auto deleteItr = m_connections.find(*(*closeSetItr));
        delete *closeSetItr;
        //deleteItr->second->close();
        delete deleteItr->second;
    }
    m_connectionCloseSet.clear();
}

//******************************************************************************
int ConnectionManager::Send (
    unsigned char * buffer, 
    unsigned bufferlen, 
    Session * session
) const {
    IConnection * connection = FindConnection(session);
    int result = 0;
    if (connection) {
        result = connection->send(buffer, bufferlen);
    }
    return result;
}

//******************************************************************************
int ConnectionManager::Broadcast (unsigned char * buffer, unsigned bufferlen) const {
    auto sendItr = m_connections.begin();

    for (; sendItr != m_connections.end(); ++sendItr) {
        sendItr->second->send(buffer, bufferlen);
    }
    return 0;
}