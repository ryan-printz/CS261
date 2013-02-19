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
#include "ConnectionManager.h"
#include <cassert>
#include <fstream>
#include <iostream>

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
            printf("%s \n", e.string);

            // echo to test receive on client side
            s_server->m_engine.Send(e, session);
        } break;

    case FILESHARE_EVENT:
        {
            FileShareEvent e;
            Packer p;
            p.unpack(e, data);
            s_server->AddFileToShare(e, session);
        } break;

    case REQUESTFILELIST_EVENT:
        {
            s_server->SendFileList(session);
        } break;
    default:
        printf("Unsupported event type '%u' sent to receive callback", eType);
		printf("Session: %s \n", s_server->m_engine.GetConnectionsInfo()->GetSessionInfo(session).c_str());
        break;
    }
}

//******************************************************************************
void SessionAcceptedCallback (HSession session) {
    assert(session != nullptr);
    assert(s_server != nullptr);

    s_server->m_sessionList[session].clear();
}

//******************************************************************************
void SessionClosedCallback (HSession session) {
    assert(session != nullptr);
    assert(s_server != nullptr);

    auto delItr = s_server->m_sessionList.find(session);

    if (delItr != s_server->m_sessionList.end())
        s_server->m_sessionList.erase(delItr);
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

//******************************************************************************
void FileShareServer::AddFileToShare (const FileShareEvent & e, HSession session) {
    auto sessionItr = m_sessionList.find(session);
        
    printf("Listing file %s for user %s.\n", e.filename, m_engine.GetConnectionsInfo()->GetSessionInfo(session).c_str());
    m_sessionList[session].push_back(std::string(e.filename));
}

//******************************************************************************
void FileShareServer::SendFileList (HSession session) {

    PrintStringEvent e;
    e.string = "*******File list begin*******";
    e.stringSize = strlen(e.string) + 1;
    m_engine.Send(e, session);

    auto sessionItr = m_sessionList.begin();
    for (; sessionItr != m_sessionList.end(); ++sessionItr) {
        //if (sessionItr->first == session)
            //continue;

        auto fileArray = sessionItr->second;
        auto fileItr = fileArray.begin();
        for (; fileItr != fileArray.end(); ++fileItr) {
            e.string = fileItr->c_str();
            e.stringSize = fileItr->size() + 1;
            m_engine.Send(e, session);
        }
    }

	e.string = "*******File list end*******\n";
    e.stringSize = strlen(e.string) + 1;
    m_engine.Send(e, session);
}