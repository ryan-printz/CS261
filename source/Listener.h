/*
*       Listener.h
*
*       by Tim Rowsey
*
*       2013/02/15
*/
#pragma once

#include <vector>

class IConnection;
class Socket;

enum ListenType {
    LISTEN_TCP,
    LISTEN_UDP,
    LISTEN_TYPES
};

class Listener {
public:
    Listener  (ListenType type, unsigned port);
    ~Listener ();

    IConnection * Listen ();

private:
    void InitializeTcp (unsigned port);
    void InitializeUdp (unsigned port);

private:
    Socket * m_socket;
    IConnection * m_connection;
    ListenType m_type;
};