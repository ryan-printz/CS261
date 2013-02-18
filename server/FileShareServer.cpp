/*
*       FileShareServer.cpp
*
*       by Tim Rowsey
*
*       2013/02/18
*/


#include "FileShareServer.h"
#include "../source/Serialization.h"
#include "../source/FileShareEvents.h"
#include <cassert>
#include <fstream>

static FileShareServer * s_server = nullptr;

// Callbacks
//******************************************************************************
void ReceiveEventCallback (HSession session, ubyte * data) {
    assert(data != nullptr);

    EventType eType = *reinterpret_cast<EventType*>(data);

    switch (eType) {
    case PRINTSTRING_EVENT:
        {
            PrintStringEvent e;
            Packer p;
            p.unpack(e, data);
            printf("Received print\n");
            printf("Session: %s \n", s_server->m_engine.GetSessionInfo(session).c_str());
            printf("%s \n", e.string);

            // echo to test receive on client side
            s_server->m_engine.Send(e, session);
        } break;
    default:
        printf("Unsupported event type '%u' sent to receive callback", eType);
        printf("Session: %s \n", s_server->m_engine.GetSessionInfo(session).c_str());
        break;
    }
}

//******************************************************************************
void SessionAcceptedCallback (HSession session) {
    assert(session != nullptr);
    assert(s_server != nullptr);

    s_server->m_sessionList.push_back(session);
}

//******************************************************************************
void SessionClosedCallback (HSession session) {
    assert(session != nullptr);
    assert(s_server != nullptr);

    s_server->m_sessionList.remove(session);
}

//******************************************************************************
FileShareServer::FileShareServer () 
    : m_engine(SessionAcceptedCallback, SessionClosedCallback, ReceiveEventCallback)
    , m_quit(false) {
    assert(s_server == nullptr);

    s_server = this;
}

//******************************************************************************
FileShareServer::~FileShareServer () {

}

//******************************************************************************
bool FileShareServer::Initialize () {
    const int SM_BUF_LEN = 16;
    char serverPort[SM_BUF_LEN];

    //create a stream and open that fucking config file
    std::fstream stream;
    stream.open("config.txt");

    //first line is ip address, so is thrown away, as is tradition
    stream.getline(serverPort, SM_BUF_LEN);
    stream.getline(serverPort, SM_BUF_LEN);

    stream.close();

    unsigned listenPort = atoi(serverPort);

    bool success = m_engine.Initialize();

    if (success)
        success = m_engine.ToggleListenTcp(listenPort);

    return success;
}

//******************************************************************************
bool FileShareServer::ShutDown () {
    m_engine.ShutDown();
    return true;
}

//******************************************************************************
bool FileShareServer::Update () {
    m_engine.Update(m_timer.Update());
    return m_quit;
}