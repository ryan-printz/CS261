/*
*       FileShareEvents.h
*
*       by Tim Rowsey
*
*       2013/02/18  
*/

#pragma once

#include "Serialization.h"

enum EventType {
    BASE_EVENT,
    PRINTSTRING_EVENT,
	GETFILEHOSTINFO_EVENT,
	LISTCONNECTIONS_EVENT,
	SHOWINFO_EVENT,
	DECORATECONNECTION_EVENT,
    FILESHARE_EVENT,
    REQUESTFILELIST_EVENT,
	FILEHOSTINFO_EVENT,
    PACKET_EVENT,
    NEWFILEINFO_EVENT,
    NEWCHUNKINFO_EVENT,
	DISCONNECT_EVENT
};

struct BaseEvent {
    BaseEvent(EventType type = BASE_EVENT) : eType(type){}
    EventType eType;

    static types def[];
};

struct PrintStringEvent : BaseEvent {
    PrintStringEvent() : BaseEvent(PRINTSTRING_EVENT) {}
    unsigned stringSize;
    const char * string;

    static types def[];
};

struct GetFileHostInfoEvent : BaseEvent 
{
	GetFileHostInfoEvent() : BaseEvent(GETFILEHOSTINFO_EVENT) {};
	unsigned fileSize;
	const char * file;
	unsigned from;
	static types def[];
};

struct ListConnectionsEvent : BaseEvent 
{
	ListConnectionsEvent() : BaseEvent(LISTCONNECTIONS_EVENT) {};
	static types def[];
};

struct ShowInfoEvent : BaseEvent 
{
	ShowInfoEvent() : BaseEvent(SHOWINFO_EVENT) {};
	unsigned connectionID;
	static types def[];
};

struct DecorateConnectionEvent : BaseEvent
{
	DecorateConnectionEvent() : BaseEvent(DECORATECONNECTION_EVENT) {};
	unsigned connectionID;
	const char * decoratorType;
	unsigned highModifier;
	unsigned lowModifier;
	static types def[];
};

struct FileShareEvent : BaseEvent
{
    FileShareEvent() : BaseEvent(FILESHARE_EVENT) {};
    unsigned nameLength;
    const char * filename;
    static types def[];
};

struct RequestFileListEvent : BaseEvent
{
    RequestFileListEvent() : BaseEvent(REQUESTFILELIST_EVENT) {};
    static types def[];
};

struct FileHostInfoEvent : BaseEvent
{
	FileHostInfoEvent() : BaseEvent(FILEHOSTINFO_EVENT) {}
    unsigned hostPort;
	unsigned fileSize;
	const char * file;
	unsigned ipSize;
	const char * hostIp;
	static types def[];
};

struct NewFileInfoEvent : BaseEvent {
    NewFileInfoEvent() : BaseEvent(NEWFILEINFO_EVENT) {}
    unsigned fileSize;
    unsigned totalChunks;
    unsigned filenameSize;
    const char * filename;
    static types def[];
};

struct PacketEvent : BaseEvent {
    PacketEvent() : BaseEvent(PACKET_EVENT) {}
    unsigned packetNum;
    unsigned totalPackets;
    unsigned chunkNum;
    unsigned filenameSize;
    const char * filename;
    unsigned packetSize;
    char * data;
    static types def[];
};

struct NewChunkInfoEvent : BaseEvent {
    NewChunkInfoEvent() : BaseEvent(NEWCHUNKINFO_EVENT) {}
    unsigned chunkNum;
    unsigned chunkSize;
    unsigned numPackets;
    unsigned filenameSize;
    const char * filename;
    static types def[];
};

struct DisconnectEvent : BaseEvent {
	DisconnectEvent() : BaseEvent(DISCONNECT_EVENT) {}

    static types def[];
};