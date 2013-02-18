/*
*       ConnectionManager.hpp
*
*       by Tim Rowsey
*
*       2013/02/09   
*/

#pragma once

#include <list>
#include <set>

struct DevSession;
typedef DevSession * HSession;
class IConnection;

class ConnectionManager {
public:
    ConnectionManager  () {}
    ~ConnectionManager ();
    
    void Add       (IConnection * connection);
    void Remove    (IConnection * connection);
    void Send      (unsigned char * buffer, unsigned bufferLen, HSession session);
    void Broadcast (unsigned char * buffer, unsigned bufferLen);

    int Receive (unsigned char * buffer, unsigned bufferLen);

    bool IsHandleValid (HSession session) const;
    
    std::string GetSessionInfo (HSession session) const;

private:
    void ClearDeleteList ();

private:
    std::list<IConnection *> m_connections;
    std::set<IConnection *> m_deleteList;
};