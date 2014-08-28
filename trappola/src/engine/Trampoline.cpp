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
#include "Trampoline.h"

///
/// Constructors / Destructor
///
NktTrampoline::NktTrampoline(const NktTrampoline& cp)
: NktVirtualCode(cp) 
{ 
	_activeCallCount = cp._activeCallCount; 
	_cleanSize = cp._cleanSize;
	_tag = cp._tag;
}
NktTrampoline::NktTrampoline(unsigned short cleanSize)
{ 
	_cleanSize = cleanSize;
	_activeCallCount = 0; 
	_tag = 0;
}

///
/// CTrampoline::Tag:
///
INT_PTR NktTrampoline::GetTag() const
{ 
	return _tag;
}
void NktTrampoline::SetTag(INT_PTR tag)
{
	_tag = tag;
}

///
/// CTrampoline::GetActiveCallCount
///
UINT NktTrampoline::GetActiveCallCount() const
{
	return _activeCallCount;
}

///
/// CTrampoline::GetCleanSize
///
USHORT NktTrampoline::GetCleanSize() const
{
	return _cleanSize;
}

///
/// CTrampoline::Initialize
///
void NktTrampoline::Initialize(const NktFunctionWrapper& handler, LPCVOID pThis)
{
	_ASSERT(!_pivot);
	//Increase counter
	Append(LOCK); Append(INC_MEM, (UINT)&_activeCallCount);

	//Save registers + flags
	Append(PUSHAD);
	Append(PUSHFD);

	//Save TEB address
	Append(MOV_EAX_FS, TEB_OFFSET);

	//Save LastError
	Append((asmcodew)0x408b, (unsigned char)0x34); // ASM: mov eax, [eax+34h]
	Append(PUSH_EAX);

	//Save ESP in EDX
	//FIXME: A function that do not preserve EDX could smash our stack.
	//FIX?: Read ESP from pushed registers.
	Append(MOV_EDX_ESP);

	//Save Tag
	Append(PUSH_INM, (UINT)_tag);

	//Create call to Handler
	NktUtils::CreateCall(handler, *this, pThis);

	//Restore UPDATED registers + flags + lastError + stack
	Append(POP_EDX);
	Append(MOV_EAX_FS, TEB_OFFSET);
	Append(ADD_EAX, TEB_LASTERROR);
	Append(MOV_EAX_PTR_EDX);
	Append(POPFD);
	Append(POPAD);

	//Decrease counter
	Append(LOCK); Append(DEC_MEM, (UINT)&_activeCallCount);

	//Clear stack and return
	(_cleanSize)? Append(RET_CLEAN, _cleanSize) : Append(RET);
}
