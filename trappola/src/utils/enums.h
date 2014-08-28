#ifndef _HOOKLIB_ENUMS_H_
#define _HOOKLIB_ENUMS_H_

#if !defined(_NKT_CALLINGCONVETIONS_DEFINED_)
	#define _NKT_CALLINGCONVETIONS_DEFINED_ 1
	/**
	Known calling conventions
	*/
	typedef enum _NktCallingConvention
	{
		cdecl_ = 0,
		pascal_ = 1,
		fastcall_ = 2,
		stdcall_ = 3,
		safecall_ = 4,
		thiscall_ = 5,
		thiscall_ms_ = 6,
		thiscall_com_ = stdcall_,
		pascal_mac_ = 17,
		syscall_ = 18,
		bad_cc_ = -1
	} NktCallingConvention;
#endif

#if !defined(_NKT_HOOKFLAGS_DEFINED_)
	#define _NKT_HOOKFLAGS_DEFINED_ 1
	/**
	Flags to alter hook behavior.
	*/
	typedef enum _NktHookFlags
	{
		_call_sync = 0,
		_call_async = 1,

		_call_before = 0x2,
		_call_after = 0x4,

		_call_before_and_after = _call_before| _call_after,

		_call_custom = 0x3,

		_call_debug = 0x8000,

		_call_invalid = 0x10000
	} NktHookFlags;
#endif //_NKT_HOOKFLAGS_DEFINED_

#endif //_HOOKLIB_ENUMS_H_