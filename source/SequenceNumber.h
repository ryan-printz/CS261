#pragma once

#include <numeric>
#include "Types.h"

#undef max
#undef min

class SequenceNumber
{
public:
	SequenceNumber() 
		: m_sequenceNumber(0) {}

	SequenceNumber(int no) 
		: m_sequenceNumber(no) {}

	SequenceNumber(const SequenceNumber& no)
		: m_sequenceNumber(no.m_sequenceNumber) {}

	operator uint() const { return m_sequenceNumber; };

	SequenceNumber& operator++() 
	{ 
		++m_sequenceNumber; 
		return *this; 
	}

	SequenceNumber operator++(int)
	{
		SequenceNumber tmp(*this);
		operator++();
		return tmp;
	}

	SequenceNumber& operator=(SequenceNumber rhs)
	{
		m_sequenceNumber = rhs.m_sequenceNumber;
		return *this;
	}

	uint m_sequenceNumber;
};

bool operator==(const SequenceNumber & lhs, const SequenceNumber & rhs);
bool operator!=(const SequenceNumber & lhs, const SequenceNumber & rhs);
bool operator >(const SequenceNumber & lhs, const SequenceNumber & rhs);
bool operator <(const SequenceNumber & lhs, const SequenceNumber & rhs);
bool operator>=(const SequenceNumber & lhs, const SequenceNumber & rhs);
bool operator<=(const SequenceNumber & lhs, const SequenceNumber & rhs);
