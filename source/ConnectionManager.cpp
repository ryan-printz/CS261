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