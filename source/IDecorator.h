#pragma once

#include <string>

class IDecorator
{
public:
	virtual std::string decoratorInfo() const 
	{ return ""; };

	// an update for the decorator.
	virtual void update(float d) {};
};
