/*
*       ConnectionManager.cpp
*
*       by Tim Rowsey
*
*       2013/02/09   
*/
#include <cassert>
#include "IConnection.h"
#include "ConnectionManager.hpp"

//******************************************************************************
//ConnectionManager::ConnectionManager () {

//}

//******************************************************************************
ConnectionManager::~ConnectionManager () {
    ClearDeleteList();
}

//******************************************************************************
void ConnectionManager::Add (IConnection * connection) {
    assert(connection != nullptr);

    m_connections.push_back(connection);
}

//******************************************************************************
void ConnectionManager::Remove (IConnection * connection) {
    assert(connection != nullptr);

    m_deleteList.insert(connection);
}

//******************************************************************************
void ConnectionManager::ClearDeleteList () {
    auto delItr = m_deleteList.begin();

    for (; delItr != m_deleteList.end(); ++delItr) {
        //(*delItr)->close();
        delete *delItr;
        m_connections.remove(*delItr);
    }

    m_deleteList.clear();
}