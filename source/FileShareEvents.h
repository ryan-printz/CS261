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
};

struct BaseEvent {
    BaseEvent(EventType type = BASE_EVENT) : eType(type){}
    EventType eType;

    static types def[];
};

struct PrintStringEvent : BaseEvent {
    PrintStringEvent() : BaseEvent(PRINTSTRING_EVENT) {}
    unsigned stringSize;
    char string [];

    static types def[];
};

