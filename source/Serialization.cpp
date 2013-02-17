#include "Serialization.h"
#include <cstring>

int m_sizeArray[6] = {	sizeof(int), sizeof(float), sizeof(char),sizeof(unsigned),
						sizeof(unsigned), sizeof(int) };
types BaseEvent::def[1] = {TYPE_END};
types TestEvent1::def[3] = {TYPE_INT, TYPE_FLOAT, TYPE_END};
types TestEvent2::def[3] = {TYPE_UARRAYSIZE, TYPE_INT, TYPE_END};


void pack(BaseEvent E, char* buf)
{
	char* current = (char*)(&E);
	
	for(int i = 0; E.def[i] != TYPE_END; ++i)
	{
		switch (E.def[i])
		{
		//case TYPE_INT:
		//	memcpy(buf, current, m_sizeArray[TYPE_INT]);
		//	buf += m_sizeArray[TYPE_INT];
		//	current += m_sizeArray[TYPE_INT];
		//	break;
		//case TYPE_FLOAT:
		//	memcpy(buf, current, m_sizeArray[TYPE_FLOAT]);
		//	buf += m_sizeArray[TYPE_FLOAT];
		//	current += m_sizeArray[TYPE_FLOAT];
		//	break;
		//case TYPE_CHAR:
		//	memcpy(buf, current, m_sizeArray[TYPE_CHAR]);
		//	buf += m_sizeArray[TYPE_CHAR];
		//	current += m_sizeArray[TYPE_CHAR];
		//	break;
		//case TYPE_UNSIGNED:
		//	memcpy(buf, current, m_sizeArray[TYPE_UNSIGNED]);
		//	buf += m_sizeArray[TYPE_UNSIGNED];
		//	current += m_sizeArray[TYPE_UNSIGNED];
		//	break;
		case TYPE_UARRAYSIZE:
			unsigned temp;
			memcpy(&temp, current, m_sizeArray[TYPE_UARRAYSIZE]);
			memcpy(buf, current, m_sizeArray[TYPE_UARRAYSIZE]);
			buf += m_sizeArray[TYPE_UARRAYSIZE];
			current += m_sizeArray[TYPE_UARRAYSIZE];
			++i;
			memcpy(buf, (char*)(*current), m_sizeArray[E.def[i]] * temp);
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