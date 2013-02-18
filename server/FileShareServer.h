/*
*       FileShareServer.h
*
*       by Tim Rowsey
*
*       2013/02/18
*/

#pragma once

#include <list>
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
    Engine m_engine;
    Timer m_timer;
    std::list<HSession> m_sessionList;

    bool m_quit;
};