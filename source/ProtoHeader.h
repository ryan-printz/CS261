#pragma once

#include "SequenceNumber.h"
#include "Types.h"

struct ProtoHeader
{
	enum Flags
	{
		PROTO_RESENT = 0x01,

		PROTO_HIGH	 = 0x80,
		PROTO_NORMAL = 0x40,
		PROTO_LOW	 = 0x20
	};

	// a constant identifier for the protocol.
	// packets that don't have this are ignored.
	ProtoHeader() : m_protocol(0xb4639620) {}
	const uint		m_protocol;

	SequenceNumber	m_sequence;
	SequenceNumber	m_ack;
	uint			m_acks;
	ubyte			m_flags;

	bool valid() { return m_protocol == 0xb4639620; };

	const static uint CONNECTION_MESSAGE = 0x5e015d13;
	const static uint KEEP_ALIVE_MESSAGE = 0x1d5e03e7;
	const static uint DISCONNECT_MESSAGE = 0xf70f8c3b;
};