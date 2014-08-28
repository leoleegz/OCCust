/*****************************************************************************
Windows Live Messenger Plugin Demo
Copyright (C) 2008  Hernán Di Pietro

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

/*****************************************************************************/
#ifndef HOOKING_H
#define HOOKING_H
#include "hooklib.h"

// function hooking structure
//
typedef struct
{
	LPCWSTR	szTargetModule;
	LPCSTR	szTargetFunction;
	USHORT	cbParam;
	void *	pvHookHandler;
	NktCallingConvention nccCallingCnv;
	int		iFlags;

} HOOK_DESCRIPTOR;

// helper macros
//
#define	PARAMETER_INDEX(x)		((size_t)hp->context.pms + (sizeof(DWORD) * (x)))
#define NUM_PARAMS(x)			(sizeof(void*) * (x))
#define	DECLARE_HOOK_HANDLER(fn) void Handle_##fn (NktHandlerParams*)
#define WINAPI_RETVAL			(hp->context.regs->EAX)

// hooking typedefs
//
typedef yasper::ptr<NktApiHook> ApiHookPtr;
typedef std::list<ApiHookPtr>	HookList;

// hooking function installer
//
void AttachHook	(const HOOK_DESCRIPTOR&,  HookList& );
void AttachHookArray (const HOOK_DESCRIPTOR[],  HookList& );

// hook handler prototypes
//
DECLARE_HOOK_HANDLER (CreateWindowExW);
DECLARE_HOOK_HANDLER (CoRegisterClassObject);
DECLARE_HOOK_HANDLER (FindResourceW);
DECLARE_HOOK_HANDLER (LoadResource);
DECLARE_HOOK_HANDLER (LockResource);
DECLARE_HOOK_HANDLER (SizeofResource);
DECLARE_HOOK_HANDLER (GetProcAddress);

// hook descriptor array
//
const HOOK_DESCRIPTOR hookArray[] = 
{
	//L"user32.dll", "CreateWindowExW", NUM_PARAMS(12),  Handle_CreateWindowExW, stdcall_, _call_after ,
	//L"ole32.dll", "CoRegisterClassObject", NUM_PARAMS(5), Handle_CoRegisterClassObject, stdcall_, _call_after,
	//L"kernel32.dll", "FindResourceW", NUM_PARAMS(3), Handle_FindResourceW, stdcall_, _call_after,
	//L"kernel32.dll", "LoadResource", NUM_PARAMS(2), Handle_LoadResource, stdcall_, _call_after,
	//L"kernel32.dll","LockResource",	NUM_PARAMS(1), Handle_LockResource, stdcall_, _call_after,
	//L"kernel32.dll","SizeofResource", NUM_PARAMS(2), Handle_SizeofResource, stdcall_, _call_after,
	//L"msftedit.dll", "CreateTextServices", NUM_PARAMS(3), Handle_CreateTextServices, stdcall_, _call_after
	L"kernel32.dll", "GetProcAddress", NUM_PARAMS(2), Handle_GetProcAddress, stdcall_, _call_after
	    
	/* the following example entry for CONTACTSUX.DLL 
	is just there to show that C++ function interception
	must be done using decorated names ...

	L"contactsux.dll", "?OnInput@CContactElement@@UAEXPAUInputEvent@DirectUI@@@Z",
		NUM_PARAMS(2), Handle_CContactElement_OnInput, thiscall_ms_ , _call_after */
};

const UINT NUM_HOOK_DESCRIPTORS = sizeof(hookArray)/sizeof(hookArray[0]);

#endif //HOOKING_H
