#ifndef _AUTOTHREADMUTE_H_
#define _AUTOTHREADMUTE_H_

#include <WTypes.h>

/**
Class to use in any scope to disable thread reports.
*/
class NktAutoThreadMute
{
public:
	NktAutoThreadMute();
	~NktAutoThreadMute();

private:
	HANDLE _hLock;
};


#endif //_AUTOTHREADMUTE_H_