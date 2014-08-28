#include "AutoThreadMute.h"
#include "HookBase.h"

///
/// Constructors / Destructor
///
NktAutoThreadMute::NktAutoThreadMute()
{
	_hLock = NktHookBase::DisableThreadReports();
}
NktAutoThreadMute::~NktAutoThreadMute()
{
	NktHookBase::RestoreThreadReports(_hLock);
}