/*
*       FileShareClient.h
*
*       by Tim Rowsey
*
*       2013/02/18
*/

#pragma once

#include <list>
#include "../source/Engine.h"
#include "../source/Timer.h"

class FileShareClient {
    friend void SessionAcceptedCallback (HSession);
    friend void SessionClosedCallback (HSession);
    friend void ReceiveEventCallback (HSession, char *);

public:
    FileShareClient ();
    ~FileShareClient ();

    bool Initialize ();
    bool ShutDown ();
    bool Update ();

private:
    Engine m_engine;
    Timer m_timer;
    bool m_quit;
};