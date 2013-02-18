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
    Engine ();
    ~Engine ();

    void Initialize ();

    const HSession ConnectTcp (char * remoteIp, unsigned remotePort);

    // returns if the port is now listening
    bool ToggleListenTcp (unsigned port);

    void Update (float dt);

    template <typename T>
    void Send (T & message, HSession session);

    // This is a temp function until we get some joe up in hurrrrrr.
    int Receive (unsigned char * buffer, unsigned bufferLen);

    //Callbacks
    typedef void (*FSessionAcceptedCallback) (HSession);
    typedef void (*FReceiveEventCallback) (HSession);
    void RegisterCallbacks (FSessionAcceptedCallback callback);

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
    FSessionAcceptedCallback f_newSession;
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