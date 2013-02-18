#pragma once

#include "Types.h"

enum types
{
	TYPE_INT,
	TYPE_FLOAT,
	TYPE_CHAR,
	TYPE_UNSIGNED,
	TYPE_UARRAYSIZE,
	TYPE_END
};

class Packer
{
public:
	template <typename T>
	void pack(T &E, ubyte ** buf, unsigned & bufferLen)
	{
        CreatePackBuffer(E, buf, bufferLen);

		ubyte * current = (ubyte*)(&E);
	
		for(int i = 0; E.def[i] != TYPE_END; ++i)
		{
			switch (E.def[i])
			{
			case TYPE_UARRAYSIZE:
				unsigned temp;
				memcpy(&temp, current, m_sizeArray[TYPE_UARRAYSIZE]);
				memcpy(*buf, current, m_sizeArray[TYPE_UARRAYSIZE]);
				*buf += m_sizeArray[TYPE_UARRAYSIZE];
				current += m_sizeArray[TYPE_UARRAYSIZE];
				++i;
				memcpy(*buf, *(ubyte**)(current), m_sizeArray[E.def[i]] * temp);
				current += 4;
				*buf += m_sizeArray[E.def[i]] * temp;
				break;
			default:
				memcpy(*buf, current, m_sizeArray[E.def[i]]);
				*buf += m_sizeArray[E.def[i]];
				current += m_sizeArray[E.def[i]];
				break;
			}
		}
	}

	template <typename T>
	void unpack(T &E, ubyte * buf)
	{
		ubyte* current = (ubyte*)(&E);
	
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

				*(ubyte**)current = new ubyte[m_sizeArray[E.def[i]] * temp];
				memcpy(*(ubyte**)current, buf, m_sizeArray[E.def[i]] * temp);
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
    template <typename T>
    void CreatePackBuffer (T &E, ubyte ** buffer, unsigned & bufferLen) 
    {
        for(int i = 0; E.def[i] != TYPE_END; ++i)
        {
            switch (E.def[i])
            {
            case TYPE_UARRAYSIZE:
                {
                    unsigned temp;
                    memcpy(&temp, *buffer, m_sizeArray[TYPE_UARRAYSIZE]);
                    bufferLen += m_sizeArray[TYPE_UARRAYSIZE] + sizeof(void*);
                    ++i;
                    break;
                }
            default:
                bufferLen += m_sizeArray[E.def[i]];
                break;
            }
        }
        unsigned paddingBytes = sizeof(void*) - (bufferLen % sizeof(void*));
        bufferLen += paddingBytes;
        
        *buffer = new unsigned char[bufferLen];
    }
	static int m_sizeArray[];
};
//Need to templatize this somehow
//void pack(BaseEvent E, char* buf);
//void unpack(BaseEvent E, char* buf);
