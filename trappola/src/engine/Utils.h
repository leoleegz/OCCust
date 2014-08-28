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
#ifndef _Utils_H_
#define _Utils_H_

#include "VirtualCode.h"
#include "SegmentRange.h"
#include "FunctionWrapper.h"

#define EV_NAME_SIZE 64

namespace NktUtils
{
	/**
	Finds the minimum set of instructions to remove to install a jump code. 
	*/
	const SIZE_T GetMinJmpSize(const DV_PTR code, SIZE_T jmp_size);

	/**
	Updates relative indirections in code.
	*/
	void UpdateIndirections(NktVirtualCode& code, const unsigned int prev_base, const unsigned int new_base);

	/**
	Update indirections in contained code and append jump to specified address.
	returns min jump size.
	*/
	void AppendUpdatedJmp(NktVirtualCode& code, const void* dest);

	/**
	Writes a memory location with the provided code.
	*/
	BOOL WriteMem(const NktVirtualCode& code, void* dest, DWORD pid = GetCurrentProcessId());

	/**
	Suspends all running threads in a process. <br>
	Calling threads is never suspended.
	@returns list id of the suspended threads.
	*/
	void SuspendThreads(NktSegmentRange range, OUT NktIdList& suspendedThreads, DWORD processId = GetCurrentProcessId());

	/**
	Resume suspended threads.
	*/
	void ResumeThreads(NktIdList& suspendedThreads);

	/**
	Creates code to make a call to a provided funtion.
	*/
	NktVirtualCode CreateCall(const NktFunctionWrapper& fnc, LPCVOID pThis = NULL, bool useJmp = false);
	void CreateCall(const NktFunctionWrapper& fnc, OUT NktVirtualCode& code, LPCVOID pThis = NULL, bool useJmp = false);

	/**
	Creates clean code for a function (if necesary).
	*/
	NktVirtualCode CreateClean(const NktFunctionWrapper& fnc);
	void CreateClean(const NktFunctionWrapper& fnc, SIZE_T size, OUT NktVirtualCode& code);

	/**
	SimpleCallFunction
	*/
	INT_PTR SimpleCallFunction(const NktFunctionWrapper& f, LPCVOID pms, SIZE_T szBytes, LPCVOID pThis = NULL);

	/**
	Call any function with the specified params.
	It will also store LastError, flags & return in EAX.
	NOTICE: regBuffer CANNOT be NULL.
	FIXME: NO __fastcall support.
	*/
	void CallFunction(const NktFunctionWrapper& fnc, NktRegisters* regBuffer, const void* params = NULL, SIZE_T szBytes = 0);

	/**
	Debug: Print registers.
	*/
	void DbgPrintRegisters(NktRegisters* reg);

	/**
	Generate a unique string for the calling thread.
	*/
	int __fastcall GetThreadString(WCHAR* buffer);
};

#endif //_Utils_H_