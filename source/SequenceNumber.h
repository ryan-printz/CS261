#pragma once

#include <numeric>
#include "Types.h"

#undef max(a, b)
#undef min(a, b)

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

	SequenceNumber& operator++() { return ++*this; }
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

inline bool operator==(const SequenceNumber & lhs, const SequenceNumber & rhs)
{
	return lhs.m_sequenceNumber == rhs.m_sequenceNumber;
}
inline bool operator!=(const SequenceNumber & lhs, const SequenceNumber & rhs) { return !operator==(lhs, rhs); }
inline bool operator>(const SequenceNumber & lhs, const SequenceNumber & rhs)
{
	// handle overflowing sequence numbers.
	// if the difference between the last number and the current number is greater
	// than half the highest possible sequence number, it's likely we overflowed.
	// in that case, reverse the comparison.
	return ((lhs.m_sequenceNumber > rhs.m_sequenceNumber)
			&& (lhs.m_sequenceNumber - rhs.m_sequenceNumber <= std::numeric_limits<uint>::max()/2))
			|| ((lhs.m_sequenceNumber < rhs.m_sequenceNumber) 
			&& (rhs.m_sequenceNumber - lhs.m_sequenceNumber > std::numeric_limits<uint>::max()/2));
}
inline bool operator <(const SequenceNumber & lhs, const SequenceNumber & rhs) { return  operator>(rhs, lhs); }
inline bool operator>=(const SequenceNumber & lhs, const SequenceNumber & rhs) { return !operator<(lhs, rhs); }
inline bool operator<=(const SequenceNumber & lhs, const SequenceNumber & rhs) { return !operator>(lhs, rhs); }
