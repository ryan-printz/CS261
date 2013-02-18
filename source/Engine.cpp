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
Engine::Engine () : f_newSession(nullptr) {
    m_connectionManager = new ConnectionManager;
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

//******************************************************************************
bool Engine::ToggleListenTcp (unsigned port) {
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
void Engine::Update (float dt) {
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
// just a temp function for chat shit
void Engine::TempPrepareChatMessage (const std::string & message, unsigned char ** buffer, unsigned & bufferLen) {
    bufferLen = message.size() + 1;
    *buffer = new unsigned char[bufferLen];
    memcpy(*buffer, message.c_str(), message.size());
    (*buffer)[bufferLen-1] = '\0';
}

//******************************************************************************
void Engine::Send (unsigned char * buffer, unsigned bufferLen, HSession session) {
    m_connectionManager->Send(buffer, bufferLen, session);
} 

//******************************************************************************
// temp for chat
int Engine::Receive (unsigned char * buffer, unsigned bufferLen ) {
    return m_connectionManager->Receive (buffer, bufferLen);
}

//******************************************************************************
void Engine::RegisterCallbacks (FSessionAcceptedCallback cb) {
    f_newSession = cb;
}