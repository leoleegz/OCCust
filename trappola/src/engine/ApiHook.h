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
#ifndef _API_HOOK_H_
#define _API_HOOK_H_

#include "VirtualCode.h"
#include "Trampoline.h"
#include "HookBase.h"
#include "HookTypes.h"

/**
CAPIHook:
Local hooking class.
*/
class NktApiHook : public NktHookBase
{
public:
	NktApiHook(const NktFunctionWrapper& fnc, const NktLocalFunction& handler, int flags = ::_call_before);
	~NktApiHook();

	/**
	Tells if a debug breakpoint (int3) was inserted in the jump code.
	*/
	BOOL GetDebug() const;

	/**
	Tells if there is a call active.
	*/
	BOOL GetActive() const;

	/**
	Function Address.
	*/
	const void* GetFunctionAddress() const;

	/**
	Unhook hooked function:
	*/
	void Unhook();

protected:
	/**
	Internal: install hook code.
	*/
	void Install(const NktFunctionWrapper& callee, const NktFunctionWrapper& handler);

	/**
	Internal: hook code initialization.
	Returns jmp size.
	*/
	UCHAR InitHookCode(const void* fnc);

	/**
	From CHookBase.
	*/
	virtual void RawCallFunction(NktHookCallContext*);

protected:
	NktTrampoline _tramp; //Trampoline code.
	NktVirtualCode _hookCode; //Code inserted in function.
	NktVirtualCode _fncCode; //Updated function's code.
	NktVirtualCode* _oldCode; //Original Function's code.
	NktFunctionWrapper _fnc;
};

#endif // _API_HOOK_H_