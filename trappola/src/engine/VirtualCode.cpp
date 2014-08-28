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
#include "VirtualCode.h"
#include "NtApi.h"

#include <Windows.h>

///
/// Constructors / Destructor
///
NktVirtualCode::NktVirtualCode(const unsigned int pages, const DWORD proc_id)
: NktAsmCode(pages * NktNtApi::GetPageSize()), _procId(proc_id)
{
	_pivot = 0;
}
NktVirtualCode::NktVirtualCode(const NktVirtualCode& cp)
: NktAsmCode(cp), _procId(cp._procId)
{
	_pivot = cp._pivot;
}

///
/// CVirtualCode::read_from
///
const SIZE_T NktVirtualCode::ReadFrom(const void* addr, const SIZE_T count)
{
	SIZE_T ret = 0;
	SIZE_T size = _pivot + count;
	SIZE_T ptr = (SIZE_T)GetAddress() + _pivot;
	if (size > GetSize())
		return 0;
	HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, _procId);
	if (ReadProcessMemory(hProc, addr, (void*)ptr, count, &ret))
	{
		_pivot += ret;
	}
	CloseHandle(hProc);
	return ret;
}

///
/// CVirtualCode::write_to
///
const BOOL NktVirtualCode::WriteTo(void* addr)
{
	if (!addr)
		return FALSE;
	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, _procId);
	BOOL ret = WriteProcessMemory(hProc, addr, (LPVOID)GetAddress(), _pivot, NULL);
	CloseHandle(hProc);
	return ret;
}

///
/// CVirtualCode::append
///
const BOOL NktVirtualCode::Append(asmcode opcode)
{
	return ReadFrom(&opcode, sizeof(asmcode)) != 0;
}
const BOOL NktVirtualCode::Append(asmcodew opcode)
{
	return ReadFrom(&opcode, sizeof(asmcodew)) != 0;
}
const BOOL NktVirtualCode::Append(asmcode opcode, unsigned int param)
{
	typedef NktAsmOpByte<unsigned int, 0> asm_;
	asm_ code;
	code.opcode = opcode;
	code.param = param;
	return ReadFrom(&code, sizeof(asm_)) != 0;
}
const BOOL NktVirtualCode::Append(asmcode opcode, unsigned short param)
{
	typedef NktAsmOpByte<unsigned short, 0> asm_;
	asm_ code;
	code.opcode = opcode;
	code.param = param;
	return ReadFrom(&code, sizeof(asm_)) != 0;
}
const BOOL NktVirtualCode::Append(asmcode opcode, unsigned char param)
{
	typedef NktAsmOpByte<unsigned char, 0> asm_;
	asm_ code;
	code.opcode = opcode;
	code.param = param;
	return ReadFrom(&code, sizeof(asm_)) != 0;
}
const BOOL NktVirtualCode::Append(asmcodew opcode, unsigned char param)
{
	typedef NktAsmOpWord<unsigned char, 0> asm_;
	asm_ code;
	code.opcode = opcode;
	code.param = param;
	return ReadFrom(&code, sizeof(asm_)) != 0;
}
const BOOL NktVirtualCode::Append(asmcodew opcode, unsigned short param)
{
	typedef NktAsmOpWord<unsigned short, 0> asm_;
	asm_ code;
	code.opcode = opcode;
	code.param = param;
	return ReadFrom(&code, sizeof(asm_)) != 0;
}
const BOOL NktVirtualCode::Append(asmcodew opcode, unsigned int param)
{
	typedef NktAsmOpWord<unsigned int, 0> asm_;
	asm_ code;
	code.opcode = opcode;
	code.param = param;
	return ReadFrom(&code, sizeof(asm_)) != 0;
}
const BOOL NktVirtualCode::Append(const NktVirtualCode& _code)
{
	return ReadFrom((LPVOID)_code.GetAddress(), _code.Pivot()) != 0;
}
const BOOL NktVirtualCode::AppendJump(DV_PTR address)
{
	unsigned int delta = address - (this->GetAddress() + _pivot + JMP_SIZE);
	return Append(JMP, delta);
}
const BOOL NktVirtualCode::AppendCall(DV_PTR address)
{
	unsigned int delta = address - (this->GetAddress() + _pivot + JMP_SIZE);
	return Append(CALL, delta);
}

///
/// CVirtualCode::raw_write
///
void NktVirtualCode::RawWrite(const unsigned int _offset, unsigned int val)
{
	DV_PTR base = GetAddress();
	_asm
	{
		push eax
		push ebx
		mov eax, base
		add eax, [_offset]
		mov ebx, val
		mov [eax], ebx
		pop ebx
		pop eax
	}
	//_pivot += sizeof(unsigned int);
}

///
/// CVirtualCode::raw_read
/// TODO: add mapping for remote process.
///
void NktVirtualCode::RawRead(unsigned int _offset, unsigned int* out_val)
{
	DV_PTR base = GetAddress();
	_asm
	{
		push esi
		push edi
		mov esi, base
		add esi, [_offset]
		mov edi, out_val
		movsd
		pop edi
		pop esi
	}
}

///
/// CVirtualCode::pivot()
///
const SIZE_T NktVirtualCode::Pivot() const
{
	return _pivot;
}

///
/// CVirtualCode::clear
///
void NktVirtualCode::Clear()
{
	_pivot = 0;
}
