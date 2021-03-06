/*
*       Listener.cpp
*
*       by Tim Rowsey
*
*       2013/02/15
*/

#include <cassert>
#include "Listener.h"
#include "TCPConnection.h"
#include "ProtoConnection.h"
#include "Socket.h"
#include "ProtoSocket.h"

//******************************************************************************
Listener::Listener (ListenType type, unsigned port) 
	: m_socket(nullptr)
	, m_connection(nullptr)
	, m_type(type) {

		switch (type) {

		case LISTEN_TCP:
			InitializeTcp(port);
			break;

		case LISTEN_UDP:
			InitializeUdp(port);
			break;
		}
}

//******************************************************************************
Listener::~Listener () {
	if (m_socket) {
		delete m_socket;
	}

	if (m_connection) {
		m_connection->cleanup();
		delete m_connection;
	}
}

//******************************************************************************
IConnection * Listener::Listen () {
	assert(m_connection != nullptr);
	assert(m_socket != nullptr);

	IConnection * newConnection = nullptr;
	// If you add a connect method for listening to UDP, we can kill 
	// this switch case, m_type, and a whole lot of shit here.
	bool success = false;
	switch (m_type) {
	case LISTEN_TCP:
		success = m_connection->accept(m_socket);
		if (success) {
			newConnection = m_connection;
			m_connection = new TCPConnection;
		}
		break;
	case LISTEN_UDP:
		success = m_connection->accept(m_socket);
		if (success) {
			newConnection = m_connection;
			m_connection = new ProtoConnection;
		}
		break;
	}

	return newConnection;
}

//******************************************************************************
void Listener::InitializeTcp (unsigned port) {

	m_socket = new Socket;
	if (m_socket == nullptr) {
		printf("Listener: Failure allocating socket.");
		return;
	}

	m_connection = reinterpret_cast<IConnection *>(new TCPConnection);

	if (!m_connection) {
		printf("Listener: Failure allocating TCPConnection");
		return;
	}

	m_socket->initializeTCP(Socket::localIP(), port);
	m_socket->bind();
	m_socket->setBlocking(false);
	m_socket->listen();
}

//******************************************************************************
void Listener::InitializeUdp (unsigned port) {
	m_socket = new ProtoSocket;
	if (m_socket == nullptr) {
		printf("Listener: Failure allocating socket.");
		return;
	}

	m_connection = reinterpret_cast<IConnection *>(new ProtoConnection);

	if (!m_connection) {
		printf("Listener: Failure allocating ProtoConnection");
		return;
	}

	reinterpret_cast<ProtoSocket*>(m_socket)->initializeProto(Socket::localIP(), port);
	m_socket->bind();
	m_socket->setBlocking(false);
	m_socket->listen();
}