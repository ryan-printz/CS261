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
#include <cstring>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <cassert>
#include "FileShareClient.h"
#include "../source/DecoratorConnectionManager.h"
#include "../source/LatencyConnectionDecorator.h"
#include "../source/DropConnectionDecorator.h"
#include "../source/InputThreading.h"
#include "../source/FileShareEvents.h"
#include "../source/Utils.hpp"

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
            printf("%s \n", e.string);
        } break;

    case PACKET_EVENT:
        {
            PacketEvent e;
            Packer p;
            p.unpack(e, data);

            s_client->InsertPacket(e, session);
        } break;

    case FILESHARE_EVENT:
        {
            FileShareEvent e;
            Packer p;
            p.unpack(e, data);
            std::string filePath;
            filePath.append(s_client->m_sharePath);
            s_client->BeginFileTransfer(e.filename, session);
        } break;

    case NEWCHUNKINFO_EVENT:
        {
            NewChunkInfoEvent e;
            Packer p;
            p.unpack(e, data);

            TransferSession temp(e.filename, session, false);

            auto tSessionItr = s_client->m_transferSessions.begin();

            for (; tSessionItr != s_client->m_transferSessions.end(); ++tSessionItr) {
                if (*tSessionItr == temp) {

                    if (!tSessionItr->m_isSender) {
                        if (tSessionItr->m_fileFrame.m_Ready == false) {
                            auto fileFrame = &tSessionItr->m_fileFrame;
                            fileFrame->m_CurrentChunk = e.chunkNum;
                            fileFrame->m_Chunk.m_TotalPackets = e.numPackets;
                            fileFrame->m_Chunk.m_Array.clear();
                            fileFrame->m_Chunk.m_Array.resize(e.chunkSize);
                            fileFrame->m_Chunk.m_InsertedArray.resize(e.chunkSize);
                        }
                        s_client->m_engine.Send(e, session);
                    }
                    tSessionItr->m_fileFrame.m_Ready = true;
                    break;
                }
            }

        } break;

    case NEWFILEINFO_EVENT:
        {
            NewFileInfoEvent e;
            Packer p;
            p.unpack(e, data);
            
            // isSender is not evaluated in comparison
            TransferSession temp(e.filename, session, false);

            auto tSessionItr = s_client->m_transferSessions.begin();

            for (; tSessionItr != s_client->m_transferSessions.end(); ++tSessionItr) {
                if (*tSessionItr == temp) {
                    tSessionItr->m_ready = true;

                    if (!tSessionItr->m_isSender) {
                        auto fileFrame = &tSessionItr->m_fileFrame;
                        fileFrame->m_FileSize = e.fileSize;
                        fileFrame->m_TotalChunks = e.totalChunks;
                        s_client->m_engine.Send(e, session);
                    }

                    break;
                }
            }

        } break;

    case FILEHOSTINFO_EVENT:
        {
            FileHostInfoEvent e;
            Packer p;
            p.unpack(e, data);
            s_client->InitiateFileTransfer(e);
        } break;

	case LISTCONNECTIONS_EVENT:
		{
			printf("%s\n", s_client->m_engine.GetConnectionsInfo()->GetAllSessionInfo().c_str());
		} break;

	case SHOWINFO_EVENT:
		{
			ShowInfoEvent * se = (ShowInfoEvent*)(data);
			HSession session = s_client->m_engine.GetConnectionsInfo()->GetHandleByIndex(se->connectionID);
			printf("%s\n", s_client->m_engine.GetConnectionsInfo()->GetSessionInfo(session).c_str());
		} break;

	case DECORATECONNECTION_EVENT:
		{
			DecorateConnectionEvent * de = (DecorateConnectionEvent*)(data);
			session = s_client->m_engine.GetConnectionsInfo()->GetHandleByIndex(de->connectionID);

			if(!session)
				break;

			DecoratorConnectionManager * dcm = (DecoratorConnectionManager*)s_client->m_engine.GetConnectionsInfo();
			if( !de->decoratorType )
			{
				dcm->Undecorate(session);
				break;
			}
			
			std::string decoratorType(de->decoratorType);
			std::transform(decoratorType.begin(), decoratorType.end(), decoratorType.begin(), ::toupper);

			if( decoratorType == "LATENCY" )
				dcm->Decorate( session, new LatencyConnectionDecorator(de->highModifier, de->lowModifier) );

			else if( decoratorType == "DROP" )
				dcm->Decorate( session, new DropConnectionDecorator(de->highModifier, de->lowModifier) );

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
	ShutDown();
}

//******************************************************************************
bool FileShareClient::Initialize () {
    if (!m_engine.Initialize())
        return false;

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
    if (!(m_tcpServer = m_engine.ConnectTcp(serverAddress, atoi(serverPort))))
        return false;

    // first, send client's UDP info as the first "filename"
	std::string clientInfo;
	clientInfo.append(m_engine.GetLocalIp());
	clientInfo.push_back(' ');
	clientInfo.append(udpListenPort);

	FileShareEvent clientInfoEvent;
	clientInfoEvent.filename = clientInfo.c_str();
	clientInfoEvent.nameLength = clientInfo.size() + 1;
	m_engine.Send(clientInfoEvent, m_tcpServer);

	// get and send actual filenames
    std::string fileList = getFilenameList(m_sharePath.c_str());

    while (fileList.size() > 0) {
        std::string fileName = getword(fileList);
        FileShareEvent e;
        e.filename = fileName.c_str();
        e.nameLength = fileName.size() + 1;
        m_engine.Send(e, m_tcpServer);
    }

    if (!m_engine.ToggleListenUdp(atoi(udpListenPort)))
        return false;

    if (!StartInputThread())
        return false;

    return true;
}

//******************************************************************************
bool FileShareClient::ShutDown () {
	DisconnectEvent e;
    m_engine.Send(e, m_tcpServer);
    StopInputThread();
    m_engine.ShutDown();
    return true;
}

//******************************************************************************
bool FileShareClient::Update () {
    // input
    std::string commandBuffer;
    CheckInput(commandBuffer);
    
    if (commandBuffer.size() != 0) {
        HandleInputCommand(commandBuffer);
    }

    // update transfer sessions
    auto tSession = m_transferSessions.begin();

    auto tTemp = tSession;
    for (; tSession != m_transferSessions.end(); ++tSession) 
	{
		if(!UpdateTransferSession(*tSession))
		{
			if(m_transferSessions.size() == 1)
			{
				m_transferSessions.clear();
				break;
			}
			
			if(tSession == m_transferSessions.begin())
			{
				while(tSession != m_transferSessions.end() && !UpdateTransferSession(*tSession))
				{
					m_transferSessions.erase(m_transferSessions.begin());
					tSession = m_transferSessions.begin();
				}
			}
			else
			{
				tTemp = tSession;
				--tTemp;
				m_transferSessions.erase(tSession);
				tSession = tTemp;
			}
		}
    }


    // update engine
    m_engine.Update(m_timer.Update());

    return m_quit;
}

//******************************************************************************
bool FileShareClient::UpdateTransferSession (TransferSession & tSession) {
    // no need to update if receiving
    if (!tSession.m_isSender)
        return true;

    FileFrame * fileFrame = &tSession.m_fileFrame;
    Chunk * chunk = &fileFrame->m_Chunk;

    if (!fileFrame->m_Ready) {

        if (!tSession.m_waitTimer.Update(m_timer.GetFrameTime()))
            return true;

        //if (++tSession.m_waitCount > m_maxWaitCount)
            // Close connection
        
        if (!tSession.m_ready) {
            NewFileInfoEvent e;
            e.filename = tSession.m_filename.c_str();
            e.filenameSize = strlen(tSession.m_filename.c_str()) + 1;
            e.fileSize = tSession.m_fileFrame.m_FileSize;
            e.totalChunks = tSession.m_fileFrame.m_TotalChunks;

            m_engine.Send(e, tSession.m_session);
            return true;
        }

        NewChunkInfoEvent e;
        e.chunkNum = tSession.m_fileFrame.m_CurrentChunk;
        e.chunkSize = tSession.m_fileFrame.m_Chunk.m_Array.size();
        e.numPackets = tSession.m_fileFrame.m_Chunk.m_TotalPackets;
        e.filename = tSession.m_filename.c_str();
        e.filenameSize = strlen(e.filename) + 1;
		m_engine.Send(e, tSession.m_session);
        return true;
    }

    // reset wait timers and variables
    tSession.m_waitCount = 0;

    if (chunk->IsSendComplete()) {
        if (fileFrame->IsFinalChunk()) {
            // we are done
            return false;
        }
        else {
            std::string filePath;
			filePath.append(m_sharePath);
			filePath.append(tSession.m_filename);
            fileFrame->LoadChunk(filePath);
            fileFrame->m_Ready = false;
        }
    }

    std::vector<char> data;
	chunk->GetNextPacket(data);
	PacketEvent e;
	e.filename = tSession.m_filename.c_str();
	e.filenameSize = tSession.m_filename.size() + 1;
	e.packetNum = chunk->m_CurrentPacket;
	e.totalPackets = chunk->m_TotalPackets;
	e.chunkNum = fileFrame->m_CurrentChunk;
	e.packetSize = data.size();
	e.data = &data.front();

	m_engine.Send(e, tSession.m_session);
	return true;
}

//******************************************************************************
void FileShareClient::InitiateFileTransfer (const FileHostInfoEvent & e) 
{
    HSession fileHost = m_engine.ConnectUdp(e.hostIp, e.hostPort);

    if (fileHost == nullptr) {
        printf("Unable to connect to host %s:%u.\n", e.hostIp, e.hostPort);
        printf("File %s transfer failed.\n", e.file);
    }

    FileShareEvent request;
    request.filename = e.file;
    request.nameLength = e.fileSize;

    // If transfer request fails, session will be removed.
    m_transferSessions.push_back(TransferSession(e.file, fileHost, false));

    m_engine.Send(request, fileHost);
}

//******************************************************************************
void FileShareClient::BeginFileTransfer (const char * filename, HSession session) {
    m_transferSessions.push_back(TransferSession(filename, session, true));
    auto newFile = &m_transferSessions.back().m_fileFrame;
    std::string filePath;
    filePath.append(m_sharePath);
    filePath.append(filename);
    newFile->LoadChunk(filePath);
    newFile->m_FileSize = newFile->getFileLength(filePath);
	if( newFile->m_FileSize == -1 )
	{
		printf("Can't send that file.\n");
		return;
	}

    NewFileInfoEvent e;
    e.filename = filename;
    e.filenameSize = strlen(filename) + 1;
    e.fileSize = newFile->m_FileSize;
    e.totalChunks = newFile->m_TotalChunks;

    m_engine.Send(e, session);
}

//******************************************************************************
void FileShareClient::InsertPacket (const PacketEvent & e, HSession session) {
    TransferSession temp(e.filename, session, false);

    auto tSession = m_transferSessions.begin();
    bool found = false;

    for (; tSession != m_transferSessions.end(); ++tSession) {
        if (*tSession == temp) {
            found = true;
            break;
        }
    }

    if (found) {
        if (tSession->m_fileFrame.m_Chunk.m_InsertedArray[e.packetNum])
            return; // already have packet saved

        std::vector<char> newPacket;
        for (unsigned i = 0; i < e.packetSize; ++i) {
            newPacket.push_back(e.data[i]);
        }

        tSession->m_fileFrame.m_Chunk.InsertPacket(newPacket, e.packetNum);

		if(tSession->m_fileFrame.m_Chunk.IsReceiveComplete())
		{
			printf("Writing Chunk %d", tSession->m_fileFrame.m_CurrentChunk);
			tSession->m_fileFrame.WriteChunk(std::string(e.filename, e.filenameSize));
		}
    }
    else {
        //remove connection
        return;
    }
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
		if(tokens[0] == "#get")
		{
			if(tokens.size() < 3)
			{
				printf("Please use the format #get <filename> <connectionID>\n");
			}
			else
			{
				GetFileHostInfoEvent e;
				e.file = tokens[1].c_str();
				e.fileSize = tokens[1].size() + 1;
				e.from = std::stoi(tokens[2]);

				m_engine.Send(e, m_tcpServer);
			}
		}
		else if(tokens[0] == "#connections")
		{
			ListConnectionsEvent e;
		
			ReceiveEventCallback(nullptr, (ubyte*)&e); 

			//m_engine.Send(e, m_tcpServer);
		}
		else if(tokens[0] == "#info")
		{
			if(tokens.size() < 2)
			{
				printf("Please use the format #info <connectionID>\n");
			}
			else
			{
				ShowInfoEvent e;
				e.connectionID = std::stoi(tokens[1]);

				ReceiveEventCallback(nullptr, (ubyte*)&e);
			}
		}
		else if(tokens[0] == "#decorate")
		{
			if(tokens.size() < 2)
			{
				printf("Please use the format #decorate <connectionID> [DROP|LATENCY] [highModifier] [lowModifier]\n");
			}
			else
			{
				DecorateConnectionEvent e;
				e.connectionID = std::stoi(tokens[1]);

				if(tokens.size() >= 4) 
				{
					e.decoratorType = tokens[2].c_str();

					e.highModifier = std::stoi(tokens[3]);

					if( tokens.size() >= 5 )
						e.lowModifier = std::stoi(tokens[4]);
				}
				if(tokens.size() < 4)
					e.decoratorType = nullptr;


				ReceiveEventCallback(nullptr, (ubyte*)&e);
			}
		}
		else if (tokens[0] == "#files")
        {
            RequestFileListEvent e;
            m_engine.Send(e, m_tcpServer);
        }
		else if (tokens[0] == "quit")
			m_quit = true;
		else
		{
			PrintStringEvent e;
			e.stringSize = command.size() + 1;
			e.string = command.c_str();

			m_engine.Send(e, m_tcpServer);
		}
	}
}