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
	GETFILE_EVENT,
	LISTCONNECTIONS_EVENT,
	SHOWINFO_EVENT,
	DECORATECONNECTION_EVENT,
    FILESHARE_EVENT,
    REQUESTFILELIST_EVENT
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

struct GetFileEvent : BaseEvent 
{
	GetFileEvent() : BaseEvent(GETFILE_EVENT) {};
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