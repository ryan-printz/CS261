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

    const HSession ConnectTcp (char * remoteIp, unsigned remotePort);

    // returns if the port is now listening
    bool ToggleListenTcp (unsigned port);
    void Update (float dt);

private:
    Engine (Engine&);
    Engine& operator= (Engine&);

private:
    ConnectionManager * m_connectionManager;
    // stores port, listener
    std::unordered_map<unsigned, Listener *> m_listenList;

};