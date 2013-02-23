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
#include "../source/Utils.hpp"
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

	case GETFILEHOSTINFO_EVENT:
		{
			GetFileHostInfoEvent e;
			Packer p;
			p.unpack(e, data);
			s_server->HandleFileHostInfoRequest(e, session);
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
    m_engine.Update((float)m_timer.Update());
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

	std::string toSend("*******File list begin*******\n");

    auto sessionItr = m_sessionList.begin();
    for (; sessionItr != m_sessionList.end(); ++sessionItr) {

        if (sessionItr->first == session)
            continue;

		char sessionIdBuff[256];
		_itoa_s((int)(sessionItr->first), sessionIdBuff, 10);

		toSend.append("SessionId: ");
		toSend.append(sessionIdBuff);
		toSend.append("\n");

        auto fileArray = sessionItr->second;
        auto fileItr = fileArray.begin();
		++fileItr; // first element is user's ip & udp listen port.
        for (; fileItr != fileArray.end(); ++fileItr) {
			toSend.append(fileItr->c_str());
			toSend.append("\n");
        }
        toSend.append("\n");
    }

	toSend.append("*******File list end*******\n");

    PrintStringEvent e;
	e.string = toSend.c_str();
	e.stringSize = toSend.size() + 1;
    m_engine.Send(e, session);
}

//******************************************************************************
void FileShareServer::HandleFileHostInfoRequest(const GetFileHostInfoEvent & e, HSession session) {
	auto sessionItr = m_sessionList.find((HSession)e.from);

	if (sessionItr == m_sessionList.end()) {
		PrintStringEvent e;
		e.string = "The client specified does not exist.";
		e.stringSize = strlen(e.string) + 1;
		m_engine.Send(e, session);
		return;
	}

	auto fileItr = sessionItr->second.begin();

	bool found = false;
	for (; fileItr != sessionItr->second.end(); ++fileItr) {
		if (!strcmp(fileItr->c_str(), e.file)) {
			found = true;
			break;
		}
	}

	if (!found) {
		PrintStringEvent e;
		e.string = "The file you requested does not exist.";
		e.stringSize = strlen(e.string) + 1;
		m_engine.Send(e, session);
		return;
	}

	std::string hostInfo = sessionItr->second[0];

	FileHostInfoEvent eT;
	eT.file = e.file;
	eT.fileSize = e.fileSize;
	std::string tempIp = getword(hostInfo);
	eT.hostIp = tempIp.c_str();
	eT.ipSize = strlen(eT.hostIp) + 1;
	eT.hostPort = atoi(getword(hostInfo).c_str());
	m_engine.Send(eT, session);
}