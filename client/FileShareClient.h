/*
*       FileShareClient.h
*
*       by Tim Rowsey
*
*       2013/02/18
*/

#pragma once

#include <string>
#include <unordered_map>
#include "../source/Engine.h"
#include "../source/Timer.h"
#include "../source/FileShareEvents.h"

struct fileInfo {
    std::string filename;
};

class FileShareClient {
    friend void SessionAcceptedCallback (HSession);
    friend void SessionClosedCallback (HSession);
    friend void ReceiveEventCallback (HSession, ubyte *);

public:
    FileShareClient ();
    ~FileShareClient ();

    bool Initialize ();
    bool ShutDown ();
    bool Update ();

private:
    void HandleInputCommand (const std::string & command);
    void InitiateFileTransfer (const StartTransferEvent & e);

private:
    Engine m_engine;
    Timer m_timer;
    bool m_quit;

    // Session management
    HSession m_tcpServer;

    std::unordered_map<fileInfo, HSession> m_transferSessions;

    std::string m_storePath;
    std::string m_sharePath;
};