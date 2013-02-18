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

template <class T>
void fuckingShit (T & s) {
	T.def[i];
}

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

class Packer
{
public:
	template <typename T>
	void pack(T &E, char* buf)
	{
		char* current = (char*)(&E);
	
		for(int i = 0; E.def[i] != TYPE_END; ++i)
		{
			switch (E.def[i])
			{
			case TYPE_UARRAYSIZE:
				unsigned temp;
				memcpy(&temp, current, m_sizeArray[TYPE_UARRAYSIZE]);
				memcpy(buf, current, m_sizeArray[TYPE_UARRAYSIZE]);
				buf += m_sizeArray[TYPE_UARRAYSIZE];
				current += m_sizeArray[TYPE_UARRAYSIZE];
				++i;
				memcpy(buf, *(char**)(current), m_sizeArray[E.def[i]] * temp);
				current += 4;
				buf += m_sizeArray[E.def[i]] * temp;
				break;
			default:
				memcpy(buf, current, m_sizeArray[E.def[i]]);
				buf += m_sizeArray[E.def[i]];
				current += m_sizeArray[E.def[i]];
				break;
			}
		}
	}

	template <typename T>
	void unpack(T &E, char* buf)
	{
		char* current = (char*)(&E);
	
		for(int i = 0; E.def[i] != TYPE_END; ++i)
		{
			switch (E.def[i])
			{
			case TYPE_UARRAYSIZE:
				{
				unsigned temp;
				memcpy(&temp, buf, m_sizeArray[TYPE_UARRAYSIZE]);
				memcpy(current, buf, m_sizeArray[TYPE_UARRAYSIZE]);
				buf += m_sizeArray[TYPE_UARRAYSIZE];
				current += m_sizeArray[TYPE_UARRAYSIZE];
				++i;

				//char* test2 = new char[m_sizeArray[E.def[i]] * temp];
				//memcpy(test2, buf, m_sizeArray[E.def[i]] * temp);

				*(char**)current = new char[m_sizeArray[E.def[i]] * temp];
				memcpy(*(char**)current, buf, m_sizeArray[E.def[i]] * temp);
				current += 4;
				buf += m_sizeArray[E.def[i]] * temp;
				break;
				}
			default:
				memcpy(current, buf, m_sizeArray[E.def[i]]);
				buf += m_sizeArray[E.def[i]];
				current += m_sizeArray[E.def[i]];
				break;
			}
		}
	}
private:
	static int m_sizeArray[];
};
//Need to templatize this somehow
void pack(BaseEvent E, char* buf);
void unpack(BaseEvent E, char* buf);
