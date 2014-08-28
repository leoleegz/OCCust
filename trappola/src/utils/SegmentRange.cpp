#include "SegmentRange.h"
#include <WTypes.h>

///
/// Constructors/Destructors
///
NktSegmentRange::NktSegmentRange(void* base, unsigned int offset)
{
	_pair = std::make_pair(base, offset);
}
NktSegmentRange::NktSegmentRange(const NktSegmentRange& r)
: _pair(r._pair) {}
NktSegmentRange::~NktSegmentRange(void) {}

///
/// SegmentRange::Contains
///
int NktSegmentRange::Contains(void* addr)
{
	UINT_PTR _addr = (UINT_PTR)(addr);
	UINT_PTR _base = (UINT_PTR)_pair.first;
	if (_addr < _base)
		return -1;
	if (_addr > (_base+_pair.second))
		return 1;
	return 0;
}
