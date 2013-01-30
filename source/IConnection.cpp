#pragma once

#include "Types.h"

class IConnection
{
public:
	virtual int send(ubyte * buffer) = 0;
	virtual int recv(ubyte * buffer) = 0;
};
