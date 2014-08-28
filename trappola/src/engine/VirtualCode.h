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
#ifndef _VirtualCode_H_ 
#define _VirtualCode_H_

#include "AsmCode.h"

/**
This class provides facilities to write assembly code on runtime.
*/
class NktVirtualCode : public NktAsmCode
{
public:
	NktVirtualCode(const unsigned int pages = 1, const DWORD proc_id = GetCurrentProcessId());
	NktVirtualCode(const NktVirtualCode&);

	/**
	Load code from an address.
	*/
	const SIZE_T ReadFrom(const void* addr, const SIZE_T count);

	/**
	Write the current code to an address.
	*/
	const BOOL WriteTo(void* addr);

	/**
	Append an instruction.
	*/
	const BOOL Append(asmcode opcode);
	const BOOL Append(asmcodew opcode);
	const BOOL Append(asmcode opcode, unsigned int param);
	const BOOL Append(asmcode opcode, unsigned short param);
	const BOOL Append(asmcode opcode, unsigned char param);
	const BOOL Append(asmcodew opcode, unsigned int param);
	const BOOL Append(asmcodew opcode, unsigned short param);
	const BOOL Append(asmcodew opcode, unsigned char param);
	const BOOL Append(const NktVirtualCode& code);
	const BOOL AppendJump(DV_PTR address);
	const BOOL AppendCall(DV_PTR address);

	/**
	Raw write in the code.
	*/
	void RawWrite(const unsigned int offset, unsigned int val);

	/**
	Raw read from the code.
	*/
	void RawRead(unsigned int offset, unsigned int* out_val);

	/**
	Current size of the code.
	*/
	const SIZE_T Pivot() const;

	/**
	Clear the code and sets the pivot at the beginning.
	*/
	void Clear();

protected:
	SIZE_T _pivot;
	const DWORD _procId;
};

#endif //_CVirtualCode_H_
