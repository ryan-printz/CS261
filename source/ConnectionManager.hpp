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

class IConnection;

class ConnectionManager {
public:
    ConnectionManager  ();
    ~ConnectionManager ();
    
    void Add    (IConnection * connection);
    void Remove (IConnection * connection);
    


private:
    void ClearDeleteList ();

private:
    std::list<IConnection *> m_connections;
    std::set<IConnection *> m_deleteList;
};