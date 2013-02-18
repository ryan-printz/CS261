/*
*       FileShareClient.cpp
*
*       by Tim Rowsey
*
*       2013/02/18
*/

#include <Windows.h>
#include <fstream>
#include "FileShareClient.h"
#include "../source/InputThreading.h"

// callbacks
void SessionAcceptedCallback (HSession session) {

}

void SessionClosedCallback (HSession session) {

}

void ReceiveEventCallback (HSession session, char * data) {

}

FileShareClient::FileShareClient () 
    : m_engine(SessionAcceptedCallback, SessionClosedCallback, ReceiveEventCallback)
    , m_quit(false) 
    , m_tcpServer(nullptr)
{

}


FileShareClient::~FileShareClient () {

}

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
    if (success)
        success = m_engine.ConnectTcp(serverAddress, atoi(serverPort));

    // start listening on udp
    //m_engine.ToggleListenUdp(atoi(udpListenPort));

    if (success)
        success = StartInputThread();

    return success;
}

bool FileShareClient::ShutDown () {
    return true;
}

bool FileShareClient::Update () {
    return true;
}