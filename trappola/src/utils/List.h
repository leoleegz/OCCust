#ifndef _IDLIST_H_
#define _IDLIST_H_

#include <list>
#include <algorithm>

/**
Extended std::list with contains check.
*/
template <class T>
class NktList : public std::list<T>
{
public:
	/**
	Simply check for a value.
	*/
	bool contains(const T& value)
	{
		return find(begin(), end(), value) != end();
	}
};

#endif //_IDLIST_H_
