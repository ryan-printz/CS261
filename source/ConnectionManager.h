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

class IConnectionInfo
{
public:
	virtual HSession GetHandleByIndex(unsigned int) const = 0;
	virtual unsigned Count() const = 0;
	virtual std::string GetSessionInfo(HSession session) const = 0;
	virtual std::string GetAllSessionInfo() const = 0;
};

class ConnectionManager : public IConnectionInfo {
public:
    ConnectionManager  () {}
    virtual ~ConnectionManager ();
    
    void Add       (IConnection * connection);
    void Remove    (IConnection * connection);
    void Send      (unsigned char * buffer, unsigned bufferLen, HSession session);
    void Broadcast (unsigned char * buffer, unsigned bufferLen);
    void Update    (float dt);

    int Receive (unsigned char * buffer, unsigned bufferLen, HSession& sessionOut);

    bool IsHandleValid (HSession session) const;

	HSession GetHandleByIndex(unsigned int) const;
	unsigned Count() const;
	std::string GetAllSessionInfo() const;
    std::string GetSessionInfo(HSession session) const;

    void ClearDeleteList ();

protected:
    std::unordered_map<HSession, IConnection *> m_connections;
    std::set<IConnection *> m_deleteList;
};