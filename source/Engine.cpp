/*
*       Engine.cpp
*
*       by Tim Rowsey
*
*       2013/02/15   
*/

#include <cassert>
#include "Engine.h"
#include "Listener.h"
#include "ConnectionManager.hpp"
#include "TCPConnection.h"

//******************************************************************************
Engine::Engine () {
    m_connectionManager = new ConnectionManager;
}

//******************************************************************************
Engine::~Engine () {
    if (m_connectionManager)
        delete m_connectionManager;

    // must clear listeners here
}

//******************************************************************************
const HSession Engine::ConnectTcp (char * remoteIp, unsigned remotePort) {
    assert(remoteIp != nullptr);

    // I shouldn't have to know anything about this at engine level
    // abstracting with factory would be wonderful

    TCPConnection * newConnection = new TCPConnection;

    if (!newConnection) {
        return HSession(nullptr);
    }

    bool success = newConnection->connect(remoteIp, remotePort);
    
    if (!success) {
        delete newConnection;
        return HSession(nullptr);
    }

    m_connectionManager->Add(reinterpret_cast<IConnection*>(newConnection));

    HSession newHandle = (HSession)newConnection;

    return newHandle;
}