/*
*       ConnectionManager.hpp
*
*       by Tim Rowsey
*
*       2013/02/09   
*/

#pragma once

#include <unordered_map>
#include <set>

class Session {
    friend class ConnectionManager;
    std::string connectionInfo;

private:
    unsigned groupAssosications;
};

class IConnection;

class ConnectionManager {
public:
    ConnectionManager  ();
    ~ConnectionManager ();

    Session * const AddConnection (IConnection * connection);
    IConnection * const FindConnection (Session * session) const;
    void CloseConnection (Session ** session);

    int Send (unsigned char * buffer, unsigned bufferlen, Session * session) const;
    int Broadcast (unsigned char * buffer, unsigned bufferlen) const;

private:
    void ClearDeleteList ();

private:
    std::unordered_map<Session &, IConnection *> m_connections;

    std::set<Session *> m_connectionCloseSet;
};