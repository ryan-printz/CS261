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
#include "DecoratorConnectionManager.h"
#include "TCPConnection.h"
#include "ProtoConnection.h"
#include "Socket.h"

//******************************************************************************
Engine::Engine (
    FSessionCallback sessionAccepted, 
    FSessionCallback sessionClosed,
    FEventReceivedCallback receiveEvent
    ) 
        : f_newSession(sessionAccepted)
        , f_closeSession(sessionClosed)
        , f_receiveEvent(receiveEvent) {
    m_connectionManager = new DecoratorConnectionManager;
}

//******************************************************************************
Engine::~Engine () {
    if (m_connectionManager)
        delete m_connectionManager;

    auto listenItr = m_listenList.begin();

    while (listenItr != m_listenList.end()) {
        if (listenItr->second != nullptr)
            delete listenItr->second;
        listenItr = m_listenList.erase(listenItr);
    }
}

//******************************************************************************
bool Engine::Initialize () {
    return initSockets(true);
}

//******************************************************************************
void Engine::ShutDown () {
    cleanSockets();
}

//******************************************************************************
HSession Engine::ConnectTcp (const char * remoteIp, unsigned remotePort) {
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

//******************************************************************************
HSession Engine::ConnectUdp (const char * remoteIp, unsigned remotePort) {
    ProtoConnection * newConnection = new ProtoConnection;

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

//******************************************************************************
bool Engine::ToggleListenTcp (unsigned port) 
{
    auto listItr = m_listenList.find(port);

    if (listItr != m_listenList.end()) {
        if (listItr->second->Type() == LISTEN_TCP) {
            m_listenList.erase(listItr);
            return false;
        }
    }

    Listener * newListener = new Listener(LISTEN_TCP, port);

    if (!newListener) {
        return false;
    }

    m_listenList[port] = newListener;

    return true;
}

//******************************************************************************
const IConnectionInfo * Engine::GetConnectionsInfo() const
{
	return m_connectionManager;
}

//******************************************************************************
bool Engine::ToggleListenUdp (unsigned port) 
{
    auto listItr = m_listenList.find(port);

    if (listItr != m_listenList.end()) {
        if (listItr->second->Type() == LISTEN_UDP) {
            m_listenList.erase(listItr);
            return false;
        }
    }

    Listener * newListener = new Listener(LISTEN_UDP, port);

    if (!newListener) {
        return false;
    }

    m_listenList[port] = newListener;

    return true;
}

//******************************************************************************
void Engine::Update (float dt) {
    // Listen & Accept new connections
    Listen();

    // Update connections;
    m_connectionManager->Update(dt);

    // Receive events
    const unsigned bufferLen = 1024;
    unsigned char buffer[bufferLen];

    HSession session = nullptr;
    m_connectionManager->Receive(buffer, bufferLen, session);

    while (session != nullptr ) {
		f_receiveEvent(session, (ubyte *)buffer);
        m_connectionManager->Receive(buffer, bufferLen, session);
    }

    // Remove disconnected sessions
    m_connectionManager->ClearDeleteList();
}

//******************************************************************************
void Engine::Listen () {
    auto listenItr = m_listenList.begin();

    for (; listenItr != m_listenList.end(); ++listenItr) {
        IConnection * newConnection = listenItr->second->Listen();

        while (newConnection != nullptr) {
            m_connectionManager->Add(newConnection);
            if (f_newSession) {
                f_newSession((HSession)newConnection);
            }
            newConnection = listenItr->second->Listen();
        }
    }
}

//******************************************************************************
void Engine::Send (unsigned char * buffer, unsigned bufferLen, HSession session) {
    if (session != nullptr)
        m_connectionManager->Send(buffer, bufferLen, session);
    else
        m_connectionManager->Broadcast(buffer, bufferLen);
} 

void Engine::removeSession(HSession session)
{
	if(f_closeSession)
		f_closeSession(session);
}
//******************************************************************************
const char * Engine::GetLocalIp () {
	return Socket::localIP();
}
