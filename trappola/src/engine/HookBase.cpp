/*
*
*  This file is
*    Copyright (C) 2006-2008 Nektra S.A.
*  
*  This program is free software; you can redistribute it and/or modify
*  it under the terms of the GNU Lesser General Public License as published by
*  the Free Software Foundation; either version 2, or (at your option)
*  any later version.
*  
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*  
*/
#include "HookBase.h"
#include "Utils.h"
#include "LocalFunction.h"
#include "NtApi.h"
#include "types.h"

///
/// Statics:
///
BOOL NktHookBase::hooksDisabled = FALSE;

///
/// Helpers:
///
#define EV_LOCK(evName) (NktNtApi::CreateEvent(NULL, FALSE, FALSE, evName))
#define EV_UNLOCK(h) ((h)? NktNtApi::CloseHandle(h) : 0)
#define EV_GET(evName) (NktNtApi::OpenEvent(EVENT_ALL_ACCESS, FALSE, evName))

///
/// Constructors / Destructor
///
NktHookBase::NktHookBase()
{
	_disabled = TRUE;
	_userData = 0;
	_flags = 0;
}
NktHookBase::~NktHookBase() {}

///
/// CAPIHook::InternalHandler [fastcall]
///
void NktHookBase::InternalHandler(NktRegisters* registers, INT_PTR tag)
{
	NktHandlerParams handlerParams;
	handlerParams.userData = _userData;
	handlerParams.teb = NktNtApi::CurrentTEB();
	handlerParams.handlerData = 0;
	handlerParams.callCookie = (INT_PTR)&handlerParams.handlerData;
	handlerParams.iHook = this;

	//Init context:
	//FIXME: functions with no param should set pms in NULL.
	NktHookCallContext* ctx = &handlerParams.context;
	ctx->pms = (void*)(registers->ESP + sizeof(INT_PTR));
	ctx->regs = registers;
	ctx->state = _ctx_none;
	ctx->tag = tag;

	//Ignore call if report is disabled:
	BOOL noreport = _disabled || hooksDisabled || !_handler.isValid() || IsThreadDisabled();
	if (noreport)
	{
		CallFunction(ctx);
		return;
	}

	if(_flags & (_call_before | _call_custom))
	{
		handlerParams.flags = _flags & (_call_before | _call_custom);
		LPCVOID pm = &handlerParams;
		_handler.Call(&pm, sizeof(pm));
	}

	if (!(_flags & _call_async) && (ctx->state == _ctx_none))
	{
		CallFunction(ctx);
	}

	if ((_flags & _call_after) && !(_flags & _call_async) && (ctx->state & _ctx_called))
	{
		handlerParams.flags = _call_after;
		LPCVOID pm = &handlerParams;
		_handler.Call(&pm, sizeof(pm));
	}
}

///
/// CAPIHook::CallFunction
///
void NktHookBase::CallFunction(NktHookCallContext* ctx)
{
	_ASSERT(ctx && !(ctx->state & (_ctx_called | _ctx_skip_call)));
	RawCallFunction(ctx);
	ctx->state |= _ctx_called;
}

///
/// CAPIHook::SkipCall
///
void NktHookBase::SkipCall(NktHookCallContext* ctx, INT_PTR retVal)
{
	//FIXME: Extend to other calling conventions. Ex: __cdecl returns in fd0 with floating point.
	ctx->regs->EAX = retVal; 
	ctx->state |= _ctx_skip_call;
}

///
/// Handler
///
void NktHookBase::SetHandler(const NktLocalFunction& fnc)
{ 
	_handler = fnc;
}
const NktFunctionWrapper& NktHookBase::GetHandler() const { return _handler; }

///
/// Flags
///
void NktHookBase::SetFlags(int f) { InterlockedExchange((LONG*)&_flags , f); }
int NktHookBase::GetFlags() const { return _flags; }

///
/// UserData
///
void NktHookBase::SetUserData(INT_PTR data) { InterlockedExchange((LONG*)&_userData, (LONG)data); }
INT_PTR NktHookBase::GetUserData() const { return _userData; }

///
/// Enabled
///
void NktHookBase::SetEnabled(BOOL b) { InterlockedExchange((LONG*)&_disabled, !b); }
BOOL NktHookBase::GetEnabled() const { return !_disabled; }

///
/// CAPIHook: Global Enabled.
///
void NktHookBase::SetGlobalEnabled(BOOL enable) { InterlockedExchange((LONG*)&hooksDisabled, !enable); }
BOOL NktHookBase::GetGlobalEnabled() { return !hooksDisabled; }

///
/// CHookBase::SetReturnValue
///
void NktHookBase::SetReturnValue(NktHookCallContext* ctx, INT_PTR retVal)
{
	_ASSERT(ctx);
	ctx->regs->EAX = retVal;
}

///
/// CHookBase::SetLastError
///
void NktHookBase::SetLastError(NktHookCallContext* ctx, INT_PTR err)
{
	_ASSERT(ctx);
	ctx->regs->lastError = err;
}

///
/// CAPIHook::DisableThreadReportes [static]
///
HANDLE NktHookBase::DisableThreadReports()
{
	WCHAR buffer[EV_NAME_SIZE];
	NktUtils::GetThreadString(buffer);
	return EV_LOCK(buffer);
}

///
/// CAPIHook::RestoreThreadReports [static]
///
void NktHookBase::RestoreThreadReports(HANDLE h)
{
	EV_UNLOCK(h);
}

///
/// CAPIHook::IsThreadDisabled [static]
///
BOOL NktHookBase::IsThreadDisabled()
{
	WCHAR buffer[EV_NAME_SIZE];
	NktUtils::GetThreadString(buffer);
	HANDLE hLock = EV_GET(buffer);
	BOOL ret = (hLock != NULL);
	EV_UNLOCK(hLock);
	return ret;
}