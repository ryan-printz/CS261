/*
*       FileShareClient.cpp
*
*       by Tim Rowsey
*
*       2013/02/18
*/

#include <Windows.h>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <cassert>
#include "FileShareClient.h"
#include "ConnectionManager.h"
#include "../source/InputThreading.h"
#include "../source/FileShareEvents.h"

FileShareClient * s_client = nullptr;

// callbacks
//******************************************************************************
void SessionAcceptedCallback (HSession session) {

}

//******************************************************************************
void SessionClosedCallback (HSession session) {

}

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
            printf("Session: %s \n", s_client->m_engine.GetConnectionsInfo()->GetSessionInfo(session).c_str());
            printf("%s \n", e.string);
        } break;

	case LISTCONNECTIONS_EVENT:
		{
			printf("%s", s_client->m_engine.GetConnectionsInfo()->GetAllSessionInfo().c_str());
		} break;

    default:
        printf("Unsupported event type '%u' sent to receive callback", eType);
        printf("Session: %s \n", s_client->m_engine.GetConnectionsInfo()->GetSessionInfo(session).c_str());
        break;
    }
}

//******************************************************************************
FileShareClient::FileShareClient () 
    : m_engine(SessionAcceptedCallback, SessionClosedCallback, ReceiveEventCallback)
    , m_quit(false) 
    , m_tcpServer(nullptr)
{
    assert(s_client == nullptr);
    s_client = this;
}

//******************************************************************************
FileShareClient::~FileShareClient () {

}

//******************************************************************************
bool FileShareClient::Initialize () {
    bool success;

    success = m_engine.Initialize();

    // read config file
    // for server tcp info
    const unsigned SM_BUF_LEN = 16;
    char serverAddress[SM_BUF_LEN];
    char serverPort[SM_BUF_LEN];

    // to read in client udp info
    char udpListenPort[SM_BUF_LEN];
    char directoryBuffer[MAX_PATH];

    ZeroMemory( serverAddress, SM_BUF_LEN );
    ZeroMemory( serverPort, SM_BUF_LEN );
    ZeroMemory( udpListenPort, SM_BUF_LEN );
    ZeroMemory( directoryBuffer, MAX_PATH );

    std::fstream stream;
    stream.open("config.txt");

    stream.getline( serverAddress, SM_BUF_LEN );
    stream.getline( serverPort   , SM_BUF_LEN );
    stream.getline( udpListenPort, SM_BUF_LEN );

    //store
    stream.getline( directoryBuffer, MAX_PATH );
    m_storePath = std::string(directoryBuffer);

    //share
    stream.getline( directoryBuffer, MAX_PATH );
    m_sharePath = std::string(directoryBuffer);


    stream.close();

    // connect to tcp server
    if (success) {
        m_tcpServer = m_engine.ConnectTcp(serverAddress, atoi(serverPort));

        if (m_tcpServer == nullptr)
            success = false;
    }

    // start listening on udp
    //m_engine.ToggleListenUdp(atoi(udpListenPort));

    if (success)
        success = StartInputThread();

    return success;
}

//******************************************************************************
bool FileShareClient::ShutDown () {
    StopInputThread();
    m_engine.ShutDown();
    return true;
}

//******************************************************************************
bool FileShareClient::Update () {
    std::string commandBuffer;
    CheckInput(commandBuffer);
    
    if (commandBuffer.size() != 0) {
        HandleInputCommand(commandBuffer);
    }

    m_engine.Update(m_timer.Update());
    return m_quit;
}

//******************************************************************************
void FileShareClient::HandleInputCommand (const std::string & command) {
    // for now this just sends a print string, but we will change this to parse commands and switch
    
	std::vector<std::string> tokens;

	std::istringstream iss(command);

	std::copy(std::istream_iterator<std::string>(iss),
         std::istream_iterator<std::string>(),
         std::back_inserter<std::vector<std::string> >(tokens));
	if(tokens.size())
	{
		if(tokens[0] == "GETFILE")
		{
			if(tokens.size() < 3)
			{
				printf("Please use the format GETFILE <filename> <connectionID>\n");
			}
			else
			{
				GetFileEvent e;
				e.file = tokens[1].c_str();
				e.fileSize = tokens[1].size() + 1;
				e.from = std::stoi(tokens[2]);

				m_engine.Send(e, m_tcpServer);
			}
		}
		else if(tokens[0] == "LISTCONNECTIONS")
		{
			ListConnectionsEvent e;
		
			ReceiveEventCallback(nullptr, (ubyte*)&e); 

			//m_engine.Send(e, m_tcpServer);
		}
		else if(tokens[0] == "SHOWINFO")
		{
			if(tokens.size() < 2)
			{
				printf("Please use the format SHOWINFO <connectionID>\n");
			}
			else
			{
				ShowInfoEvent e;
				e.connectionID = std::stoi(tokens[1]);

				m_engine.Send(e, m_tcpServer);
			}
		}
		else
		{
			PrintStringEvent e;
			e.stringSize = command.size() + 1;
			e.string = command.c_str();

			m_engine.Send(e, m_tcpServer);
		}
	}
}