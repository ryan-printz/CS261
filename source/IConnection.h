#pragma once

#include "Types.h"
#include <string>

class IConnection
{
public:
	virtual int send(ubyte * buffer, int bufferlen) = 0;
	virtual int recv(ubyte * buffer, int bufferlen) = 0;
	virtual std::string connectionInfo() const = 0;
};
