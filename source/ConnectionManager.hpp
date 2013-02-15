/*
*       ConnectionManager.hpp
*
*       by Tim Rowsey
*
*       2013/02/09   
*/

#pragma once

#include <vector>
#include <set>

class IConnection;

class ConnectionManager {
public:
    ConnectionManager  ();
    ~ConnectionManager ();
    

private:


    std::vector<IConnection *> m_udpConnections;
};