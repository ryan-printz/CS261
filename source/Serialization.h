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
	void pack(T &E, ubyte ** buffer, unsigned & bufferLen)
	{
        CreatePackBuffer(E, buffer, bufferLen);

        ubyte * buf = (*buffer) + 4;
		**(int**)buffer = bufferLen;
		ubyte * current = (ubyte*)(&E);
	
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
				memcpy(buf, *(ubyte**)(current), m_sizeArray[E.def[i]] * temp);
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
		bufferLen += 4;
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

				//*(ubyte**)current = new ubyte[m_sizeArray[E.def[i]] * temp];
				//memcpy(*(ubyte**)current, buf, m_sizeArray[E.def[i]] * temp);

				*(ubyte**)current = buf;

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
        unsigned offset = 0;
        bufferLen = 0;
        for(int i = 0; E.def[i] != TYPE_END; ++i)
        {
            switch (E.def[i])
            {
            case TYPE_UARRAYSIZE:
                {
                    unsigned temp;
                    memcpy(&temp, (ubyte*)(&E) + offset, m_sizeArray[TYPE_UARRAYSIZE]);
                    bufferLen += m_sizeArray[TYPE_UARRAYSIZE];
                    offset += m_sizeArray[TYPE_UARRAYSIZE];
                    ++i;
                    bufferLen += m_sizeArray[E.def[i]]*temp;
                    offset += sizeof(void*);
                    break;
                }
            default:
                bufferLen += m_sizeArray[E.def[i]];
                offset += m_sizeArray[E.def[i]];
                break;
            }
        }
        unsigned paddingBytes = sizeof(void*) - (bufferLen % sizeof(void*));
        bufferLen += paddingBytes;
        
        (*buffer) = new ubyte[bufferLen + 4];
    }
	static int m_sizeArray[];
};
//Need to templatize this somehow
//void pack(BaseEvent E, char* buf);
//void unpack(BaseEvent E, char* buf);
