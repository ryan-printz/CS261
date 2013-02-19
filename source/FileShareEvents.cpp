/*
*       FileShareEvents.cpp
*
*       by Tim Rowsey
*
*       2013/02/18
*/

#include "FileShareEvents.h"

types BaseEvent::def[] = {
    TYPE_UNSIGNED, 
    TYPE_END
};

types PrintStringEvent::def[] = {
    TYPE_UNSIGNED,
    TYPE_UARRAYSIZE, TYPE_CHAR,
    TYPE_END
};

types GetFileEvent::def[] = {
    TYPE_UNSIGNED, 
    TYPE_UARRAYSIZE, TYPE_CHAR,
    TYPE_UNSIGNED,
    TYPE_END
};

types ListConnectionsEvent::def[] = {
    TYPE_UNSIGNED, 
    TYPE_END
};

types ShowInfoEvent::def[] = {
    TYPE_UNSIGNED, 
	TYPE_UNSIGNED,
    TYPE_END
};

types DecorateConnectionEvent::def[] = {
	TYPE_UNSIGNED,
	TYPE_UNSIGNED,
	TYPE_UARRAYSIZE, TYPE_CHAR,
	TYPE_UNSIGNED,
	TYPE_UNSIGNED,
	TYPE_END
};

types FileShareEvent::def[] = {
    TYPE_UNSIGNED,
    TYPE_UARRAYSIZE, TYPE_CHAR,
    TYPE_END
};

types RequestFileListEvent::def[] = {
    TYPE_UNSIGNED,
    TYPE_END
};

types StartTransferEvent::def[] = {
	TYPE_UNSIGNED,
	TYPE_UARRAYSIZE, TYPE_CHAR,
	TYPE_UARRAYSIZE, TYPE_CHAR,
	TYPE_UNSIGNED,
	TYPE_END
};
