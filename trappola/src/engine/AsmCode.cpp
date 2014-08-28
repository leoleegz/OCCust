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
#include "AsmCode.h"

#ifdef _DEBUG
	#define INIT_OP INT3
#else
	#define INIT_OP NOP
#endif

///
/// Constructors / Destructors
///
NktAsmCode::NktAsmCode(const unsigned int size)
{ 
	_size = size;
	_code = NULL;
	_refCount = new size_t;
	*_refCount = 1;
	if (_size)
		_code = (asmcode*)VirtualAlloc(NULL, sizeof(asmcode)*size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (!_code)
		_size = 0;
	else
		memset(_code, INIT_OP, sizeof(asmcode)*size);
}
NktAsmCode::NktAsmCode(const NktAsmCode& cp)
{
	_size = cp._size;
	_code = cp._code;
	_refCount = cp._refCount;
	InterlockedIncrement((LONG*)_refCount);
}
NktAsmCode::~NktAsmCode()
{
	if (!_code || !_refCount)
		return;

	if (InterlockedDecrement((LONG*)_refCount))
		return;

	VirtualFree(_code, 0, MEM_RELEASE);
	_code = NULL;
	_size = 0;
	delete _refCount;
	_refCount = NULL;
}

///
/// Address
///
DV_PTR NktAsmCode::GetAddress() const
{
	return (DV_PTR) _code;
}

///
/// Size
///
SIZE_T NktAsmCode::GetSize() const
{
	return _size;
}