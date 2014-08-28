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
#ifndef _VTHOOK_H_
#define _VTHOOK_H_

#include "HookBase.h"
#include "types.h"
#include <map>

/**
Hooks a Virtual Table.
*/
class NktVtHook : public NktHookBase
{
public:
	typedef struct EntryData;
	typedef std::map<SIZE_T, EntryData*> Entries;

public:
	NktVtHook(const DV_PTR vt, int cc = thiscall_com_);
	~NktVtHook();

	/**
	Get current function address in VT.
	*/
	DV_PTR GetAddress(SIZE_T index);

	/**
	Get Original function address in VT.
	*/
	DV_PTR GetOriginalAddress(SIZE_T index);

	/**
	Hook a VT Entry.
	@index entry to install into.
	*/
	void HookEntry(SIZE_T index, USHORT pmsSize = 0);

	/**
	Unhook and restore VT entry with original value. Entry must have been hooked.
	@index entry to restore. User INVALID_INDEX to restore all.
	*/
	void UnhookEntry(SIZE_T index = INVALID_INDEX);

protected:
	/**
	From CHookBase.
	*/
	virtual void RawCallFunction(NktHookCallContext*);

	void UnhookEntry(EntryData*);

protected:
	const DV_PTR _vt;
	const NktCallingConvention _cc;
	Entries _entries;
};

#endif //_VTHOOK_H_