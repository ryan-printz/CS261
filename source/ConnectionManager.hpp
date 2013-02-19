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

struct DevSession;
typedef DevSession * HSession;
class IConnection;

class ConnectionManager {
public:
    ConnectionManager  () {}
    virtual ~ConnectionManager ();
    
    void Add       (IConnection * connection);
    void Remove    (IConnection * connection);
    void Send      (unsigned char * buffer, unsigned bufferLen, HSession session);
    void Broadcast (unsigned char * buffer, unsigned bufferLen);

    int Receive (unsigned char * buffer, unsigned bufferLen, HSession& sessionOut);

    bool IsHandleValid (HSession session) const;
    
    std::string GetSessionInfo (HSession session) const;
    void ClearDeleteList ();

protected:
    std::unordered_map<HSession, IConnection *> m_connections;
    std::set<IConnection *> m_deleteList;
};