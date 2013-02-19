/*
*       Engine.h
*
*       by Tim Rowsey
*
*       2013/02/15   
*/

#pragma once


#include <unordered_map>
#include "Types.h"
#include "Serialization.h"

typedef struct DevSession {} * HSession;

class IConnectionInfo;
class ConnectionManager;
class Listener;

class Engine {
public:
    // Callback Types
    typedef void (*FSessionCallback) (HSession);
    typedef void (*FEventReceivedCallback) (HSession session, ubyte * data);

    Engine (
        FSessionCallback sessionAccepted, 
        FSessionCallback sessionClosed,
        FEventReceivedCallback receiveEvent
    );
    ~Engine ();
    
    // engine 
    bool Initialize ();
    void Update (float dt);
    void ShutDown ();

	const IConnectionInfo * GetConnectionsInfo() const;

    HSession ConnectTcp (char * remoteIp, unsigned remotePort);
    bool ToggleListenTcp (unsigned port);

    HSession ConnectUdp (char * remoteIp, unsigned remotePort);
    bool ToggleListenUdp (unsigned port);

    template <typename T>
    void Send (T & event, HSession session);

    const char * GetLocalIp ();

private:
    void Listen ();
    void Send(ubyte * buffer, unsigned bufferLen, HSession session);

    // No copy
    Engine (Engine&);
    Engine& operator= (Engine&);

private:
    ConnectionManager * m_connectionManager;
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

    Packer p;
    p.pack(message, &buffer, bufferLen);

    Send(buffer, bufferLen, session);

    delete [] buffer;
}