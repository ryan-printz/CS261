/*
*       Engine.h
*
*       by Tim Rowsey
*
*       2013/02/15   
*/

#pragma once

#include <list>

typedef struct DevSession {} * HSession;


class ConnectionManager;
class Listener;


class Engine {
public:
    Engine ();
    ~Engine ();


private:
    Engine (Engine&);
    Engine& operator= (Engine&);

    const HSession ConnectTcp (char * remoteIp, unsigned remotePort);
private:
    ConnectionManager * m_connectionManager;
    std::list<Listener *> m_listenList;

};