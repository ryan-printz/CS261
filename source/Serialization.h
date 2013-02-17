#pragma once


enum types
{
	TYPE_INT,
	TYPE_FLOAT,
	TYPE_CHAR,
	TYPE_UNSIGNED,
	TYPE_UARRAYSIZE,
	TYPE_END
};



class BaseEvent
{
public:
	static types def[];
};



class TestEvent1 : public BaseEvent
{
public:
	int test1;
	float test2;

	static types def[];
};


class TestEvent2 : public BaseEvent
{
public:
	unsigned test1;
	int* test2;

	static types def[];
};

void pack(BaseEvent E, char* buf);
