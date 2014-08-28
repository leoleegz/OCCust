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
#ifndef _TRAMPOLINE_H_
#define _TRAMPOLINE_H_

#include "VirtualCode.h"
#include "Utils.h"

/**
Trampoline code.
*/
class NktTrampoline : public NktVirtualCode
{
friend class NktApiHook;

public:
	typedef struct CallContext;

public:
	NktTrampoline(unsigned short cleanSize = 0);
	NktTrampoline(const NktTrampoline&);

	/**
	Number of entrance in this functions.
	*/
	UINT GetActiveCallCount() const;

	/**
	Param cleaning size.
	*/
	USHORT GetCleanSize() const;

	/**
	Get custom tag.
	*/
	INT_PTR GetTag() const;

	/**
	Set custom tag
	*/
	void SetTag(INT_PTR);

	/**
	Initialize trampoline code with specified handler. Call only once.
	@handler is expected to be as follows:
	void class::*(DV_REGISTERS*, INT_PTR tag)
	*/
	void Initialize(const NktFunctionWrapper& handler, LPCVOID pThis = NULL);

protected:
	unsigned int _activeCallCount;
	unsigned short _cleanSize;
	INT_PTR _tag;
};

#endif _TRAMPOLINE_H_