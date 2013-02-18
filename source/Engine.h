/*
*       Engine.h
*
*       by Tim Rowsey
*
*       2013/02/15   
*/

#pragma once

#include <unordered_map>

typedef struct DevSession {} * HSession;

class ConnectionManager;
class Listener;

class Engine {
public:
    // Callback Types
    typedef void (*FSessionCallback) (HSession);
    typedef void (*FEventReceivedCallback) (HSession session, char * data);

    Engine (
        FSessionCallback sessionAccepted, 
        FSessionCallback sessionClosed,
        FEventReceivedCallback receiveEvent
    );
    ~Engine ();
    
    std::string GetSessionInfo (HSession session) const;

    bool Initialize ();
    HSession ConnectTcp (char * remoteIp, unsigned remotePort);
    bool ToggleListenTcp (unsigned port);
    void Update (float dt);

    template <typename T>
    void Send (T & message, HSession session);

    // This is a temp function until we get some joe up in hurrrrrr.
    int Receive (unsigned char * buffer, unsigned bufferLen);

private:
    // this is a temp message to pack a string
    void TempPrepareChatMessage (const std::string & message, unsigned char ** buffer, unsigned & bufferLen);
    void Send(unsigned char * buffer, unsigned bufferLen, HSession session);

    Engine (Engine&);
    Engine& operator= (Engine&);

private:
    ConnectionManager * m_connectionManager;
    // stores port, listener
    std::unordered_map<unsigned, Listener *> m_listenList;

    //Callbacks
    FSessionCallback       f_newSession;
    FSessionCallback       f_closeSession;
    FEventReceivedCallback f_receiveEvent;
};

template <typename T>
void Engine::Send (T & message, HSession session) {
    ubyte * buffer = nullptr;
    unsigned bufferLen = 0;

    // Joe, do something like this to translate message for sending
    // replace this shit with your stuff in your own file
    // shouldn't look for just std::string, our messages can be whatever you want
    // this is just bad code for you to replace
    TempPrepareChatMessage(message, &buffer, bufferLen);

    if (!buffer) {
        return;
    }

    Send(buffer, bufferLen, session);

    delete buffer;
}