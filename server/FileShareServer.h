/*
*       FileShareServer.h
*
*       by Tim Rowsey
*
*       2013/02/18
*/

#pragma once

#include <unordered_map>
#include <vector>
#include "../source/FileShareEvents.h"
#include "../source/Engine.h"
#include "../source/Timer.h"

class FileShareServer {
friend void SessionAcceptedCallback (HSession);
friend void SessionClosedCallback (HSession);
friend void ReceiveEventCallback (HSession, ubyte *);

public:
    FileShareServer ();
    ~FileShareServer ();

    bool Initialize ();
    bool ShutDown ();
    bool Update ();

private:
    void AddFileToShare (const FileShareEvent & e, HSession session);
	void HandleFileRequest(const GetFileEvent & e, HSession session);
    void SendFileList (HSession session);
private:
    Engine m_engine;
    Timer m_timer;

    // session, vector of available files
    std::unordered_map<HSession, std::vector<std::string> > m_sessionList;

    bool m_quit;
};