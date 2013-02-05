#pragma once

#include "Types.h"
#include <string>

class IConnection
{
public:
	virtual int send(ubyte * buffer, uint bufferlen) = 0;
	virtual int receive(ubyte * buffer, uint bufferlen) = 0;
	virtual std::string connectionInfo() const = 0;
};
