#include "SequenceNumber.h"

bool operator==(const SequenceNumber & lhs, const SequenceNumber & rhs)
{
	return lhs.m_sequenceNumber == rhs.m_sequenceNumber;
}

bool operator!=(const SequenceNumber & lhs, const SequenceNumber & rhs)
{ return !operator==(lhs, rhs); }

bool operator>(const SequenceNumber & lhs, const SequenceNumber & rhs)
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

bool operator <(const SequenceNumber & lhs, const SequenceNumber & rhs) 
{ return  operator>(rhs, lhs); }

bool operator>=(const SequenceNumber & lhs, const SequenceNumber & rhs) 
{ return !operator<(lhs, rhs); }

bool operator<=(const SequenceNumber & lhs, const SequenceNumber & rhs) 
{ return !operator>(lhs, rhs); }