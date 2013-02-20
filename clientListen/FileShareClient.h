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
#include "../source/FileFrame.h"

class TransferSession {
public:
    TransferSession (const char * filename, HSession session, bool isSender)
        : m_session(session)
        , m_filename(filename)
        , m_isSender(isSender)
        , m_ready(false)
        , m_waitTimer(0, 1)
        , m_waitCount(0)
        , m_maxWaitCount(10) {}
    
    bool operator== (const TransferSession & rhs) { return (m_session == rhs.m_session 
        && m_filename == rhs.m_filename);}

    // all data is public because honey badger.
    HSession m_session;
    std::string m_filename;
    bool m_isSender;
    FileFrame m_fileFrame;
    bool m_ready;
    TimeInterp m_waitTimer;
    unsigned m_waitCount;
    unsigned m_maxWaitCount;
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
    void InitiateFileTransfer (const FileHostInfoEvent & e);
    void BeginFileTransfer (const char * filename, HSession session);
    void UpdateTransferSession (TransferSession & tSession);

private:
    Engine m_engine;
    Timer m_timer;
    bool m_quit;

    // Session management
    HSession m_tcpServer;

    std::vector<TransferSession> m_transferSessions;

    std::string m_storePath;
    std::string m_sharePath;
};
