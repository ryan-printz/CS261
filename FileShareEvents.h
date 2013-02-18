/*
*       FileShareEvents.h
*
*       by Tim Rowsey
*
*       2013/02/18  
*/

#pragma once

enum EventType {
    BASE_EVENT,
    PRINTSTRING_EVENT,

};

struct BaseEvent {
    BaseEvent(EventType type = BASE_EVENT) : eType(type){}
    EventType eType;
};

struct PrintStringEvent : BaseEvent {
    PrintStringEvent() : BaseEvent(PRINTSTRING_EVENT) {}
    char string [];
};

