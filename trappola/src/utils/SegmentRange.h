#ifndef _SEGMENT_RANGE_
#define _SEGMENT_RANGE_

#include <utility>

/**
Class intended to identify a code segment.
*/
class NktSegmentRange
{
public:
	NktSegmentRange(void* base, unsigned int offset);
	NktSegmentRange(const NktSegmentRange&);
	~NktSegmentRange();

	/**
	Checks if an address is in the range of the segment.
	*/
	int Contains(void* addr);

private:
	std::pair<void*, unsigned int> _pair;
};

#endif //_SEGMENT_RANGE_

