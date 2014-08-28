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
#include "ApiHook.h"
#include "utils.h"
#include "LocalFunction.h"

///
/// Helpers:
///
typedef void (__fastcall NktApiHook::* Method)(NktRegisters*, INT_PTR);
typedef union {void* fnc; Method member; } method_ptr;

///
/// Constructors / Destructor
///
NktApiHook::NktApiHook(const NktFunctionWrapper& fnc, const NktLocalFunction& fw, int flags)
: _fnc(fnc), _tramp(fnc.GetCalleeCleanSize())
{
	NktHookBase::SetHandler(fw);
	NktHookBase::SetFlags(flags);
	NktHookBase::SetUserData(NULL);
	NktHookBase::SetEnabled(FALSE);

	//Settings:
	_oldCode = NULL;

	method_ptr member;
	member.member = &NktApiHook::InternalHandler;
	NktFunctionWrapper fh(member.fnc, fastcall_, REGISTER_SIZE << 1);
	_tramp.Initialize(fh, this);

	//Install hook:
	Install(fnc, fw);

	SetEnabled(TRUE);
}
NktApiHook::~NktApiHook()
{
	Unhook();
}

///
/// CAPIHook::Install
///
void NktApiHook::Install(const NktFunctionWrapper& callee, const NktFunctionWrapper& handler)
{
	NktIdList suspendedThreads;
	NktVirtualCode calleeCode;
	DV_PTR fncCont = NULL;
	void* fncAddr = (void*)_fnc.GetAddress();

	//Init hook code:
	UCHAR jmpSize = InitHookCode(callee.GetAddress());

	//Call Function Code:
	_fncCode.ReadFrom((const void*)_oldCode->GetAddress(), _oldCode->Pivot());
	NktUtils::AppendUpdatedJmp(_fncCode, fncAddr);

	//Suspend threads before writing jump:
	NktSegmentRange range(fncAddr, jmpSize);
	NktUtils::SuspendThreads(range, suspendedThreads);

	//Write jump to new code in original function:
	NktUtils::WriteMem(_hookCode, fncAddr);

	//Resume threads:
	NktUtils::ResumeThreads(suspendedThreads);
}

///
/// CAPIHook::CallFunction
///
void NktApiHook::RawCallFunction(NktHookCallContext* ctx)
{
	USHORT pmSize = _fnc.GetParamSize();
	NktFunctionWrapper f((const void*)_fncCode.GetAddress(), _fnc.GetCallConvention(), pmSize);
	NktLocalFunction lf (f);
	lf.Call(ctx->regs, ctx->pms, pmSize);
}


///
/// Hook::InitHookCode
///
UCHAR NktApiHook::InitHookCode(const void* fncAddr)
{
	//Debug:
	if ((GetFlags() & _call_debug) == _call_debug)
		_hookCode.Append(INT3);

	//Jump to our code:
	NktAsmOpByte<unsigned int, JMP> jmp;
	jmp.param = _tramp.GetAddress() - ((DV_PTR)fncAddr + sizeof(jmp) + _hookCode.Pivot());
	_hookCode.ReadFrom(&jmp, sizeof(jmp));

	//Calculate jmp:
	UCHAR jmpSize = (UCHAR)NktUtils::GetMinJmpSize((DV_PTR)fncAddr, _hookCode.Pivot());

	//Save old code for unhook:
	_oldCode = new NktVirtualCode(jmpSize);
	if(_oldCode->ReadFrom(fncAddr, jmpSize) != jmpSize)
	{
		OutputDebugStringW(L"APIHook: Initialize: Error ReadFrom.");
		delete _oldCode;
		_oldCode = NULL;
		return 0;
	}

	return jmpSize;
}

///
/// CAPIHook::Unhook
///
void NktApiHook::Unhook()
{
	if(_oldCode)
	{
		//Prevent any report action.
		NktHookBase::SetFlags(0);

		//Suspend threads:
		NktIdList suspendedThreads;
		NktSegmentRange range((void*) _oldCode->GetAddress(), _oldCode->GetSize());
		NktUtils::SuspendThreads(range, suspendedThreads);

		//Write jump to new code in original function:
		NktUtils::WriteMem(*_oldCode, (void*)_fnc.GetAddress());

		//Resume threads:
		NktUtils::ResumeThreads(suspendedThreads);

		// TODO FIXME: if the thread is before the INC instruction it could remove the hook while the code is being
		// executed. It's a weird race condition and very small and I have never experimented it.
		while(_tramp.GetActiveCallCount())
		{
			Sleep(1);
		}

		delete _oldCode;
		_oldCode = NULL;
	}
}

///
/// CAPIHook: Active.
///
BOOL NktApiHook::GetActive() const
{
	return _tramp.GetActiveCallCount() > 0;
}

///
/// CAPIHook: Function Address.
///
const void* NktApiHook::GetFunctionAddress() const
{
	return _fnc.GetAddress();
}