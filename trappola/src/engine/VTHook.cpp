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
#include "Memory.h"
#include "VTHook.h"
#include "Trampoline.h"

///
/// EntryData
///
struct NktVtHook::EntryData
{
	DV_PTR prevAddress;
	DV_PTR index;
	DV_PTR pmsSize;
	NktTrampoline tramp;

	EntryData(USHORT cleanSize) : tramp(cleanSize) {}
};

///
/// Helpers:
///
typedef void (__fastcall NktVtHook::* Method)(NktRegisters*, INT_PTR);
typedef union {void* ptr; Method member; } method_ptr;

///
/// Constructors / Destructor
///
NktVtHook::NktVtHook(const DV_PTR vt, int cc)
: _vt(vt), _cc(static_cast<NktCallingConvention>(cc)) { _ASSERT(cc == thiscall_ms_ || cc == thiscall_ || cc == thiscall_com_); }
NktVtHook::~NktVtHook() {}

///
/// CVTHook::HookEntry
///
void NktVtHook::HookEntry(SIZE_T index, USHORT pmsSize)
{
	//Check entry:
	if (_entries.find(index) != _entries.end())
	{
		OutputDebugStringW(L"VTHook: HookEntry: Entry already hooked.");
		return;
	}

	NktMemory mem(_vt);
	DV_PTR prev = 0;
	SIZE_T pos = index * sizeof(DV_PTR);

	//Obtain previous entry:
	mem.Read(pos, sizeof(DV_PTR), (DV_PTR)&prev);

	//Entry data:
	USHORT cleanSize = (_cc == thiscall_ms_)? pmsSize-/*pThis*/sizeof(DV_PTR) : pmsSize; //FIXME: support __fastcall
	EntryData* entry = new EntryData(cleanSize);
	entry->index = index;
	entry->prevAddress = prev;
	entry->pmsSize = (_cc == thiscall_ms_)? pmsSize - /*pThis*/sizeof(DV_PTR) : pmsSize;
	Entries::value_type v(index, entry);
	_entries.insert(v);

	//Init trampoline:
	NktTrampoline& tramp = entry->tramp;
	method_ptr member;
	member.member = &NktVtHook::InternalHandler;
	NktFunctionWrapper fh(member.ptr, fastcall_, REGISTER_SIZE << 1);
	tramp.SetTag((DV_PTR)entry);
	tramp.Initialize(fh, this);

	//Write new address:
	DV_PTR p = tramp.GetAddress();
	mem.Write(pos, sizeof(DV_PTR), &p);
}

///
/// CVTHook::GetAddress
///
DV_PTR NktVtHook::GetAddress(SIZE_T index)
{
	return _vt + index * sizeof(DV_PTR);
}

///
/// CVTHook::OldValue
///
DV_PTR NktVtHook::GetOriginalAddress(SIZE_T index)
{
	DV_PTR ret;
	Entries::iterator it = _entries.find(index);
	if (it != _entries.end())
	{
		EntryData* entry = it->second;
		ret = entry->prevAddress;
	}
	else
		ret = *((DV_PTR*)_vt + index);
	return ret;
}

///
/// CVTHook::UnhookEntry
///
void NktVtHook::UnhookEntry(SIZE_T index /*= INVALID_INDEX*/)
{
	//Restore all:
	Entries::iterator end = _entries.end();
	if (index == INVALID_INDEX)
	{
		for (Entries::const_iterator it = _entries.begin(); it != end; ++it)
			UnhookEntry(it->second);
		_entries.clear();
	}
	//Restore index
	else
	{
		Entries::iterator it = _entries.find(index);
		if (it == _entries.end())
		{
			WCHAR buffer[64];
			wsprintf(buffer, L"VTHook: RestoreEntry: Entry was not hooked: %i", index);
			OutputDebugStringW(buffer);
			return;
		}
		UnhookEntry(it->second);
		_entries.erase(it);
	}

}
void NktVtHook::UnhookEntry(EntryData* entry)
{
	//Update VT and remove entry:
	SIZE_T pos = entry->index * sizeof(DV_PTR);
	NktMemory mem(_vt);
	DV_PTR prev = entry->prevAddress;
	mem.Write(pos, sizeof(DV_PTR), (DV_PTR)&prev);
	delete entry;
}


///
/// CHookBase::RawCallFunction [virtual]
///
void NktVtHook::RawCallFunction(NktHookCallContext* ctx)
{
	EntryData* entry = (EntryData*)ctx->tag;
	NktFunctionWrapper fw((void*)entry->prevAddress, _cc, entry->pmsSize);
	NktLocalFunction lf(fw);
	lf.Call(ctx->regs, ctx->pms, entry->pmsSize);
}