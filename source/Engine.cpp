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
            newConnection = listenItr->second->Listen();
        }
    }
}

// just a temp function for chat shit
void PrepareChatMessage (const std::string & message, unsigned char ** buffer, unsigned & bufferLen) {
    bufferLen = message.size() + 1;
    *buffer = new unsigned char[bufferLen];
    memcpy(*buffer, message.c_str(), message.size());
    *buffer[message.size()] = '\0';
}

//******************************************************************************
template <typename T>
void Engine::Send (T * message, HSession session) {
    ubyte * buffer = nullptr;
    unsigned bufferLen = 0;

    // Joe, do something like this to translate message for sending
    // replace this shit with your stuff in your own file
    // shouldn't look for just std::string, our messages can be whatever you want
    // this is just bad code for you to replace
    PrepareChatMessage(*message, &buffer, &bufferlen);

    m_connectionManager->Send(buffer, bufferLen, session);

}