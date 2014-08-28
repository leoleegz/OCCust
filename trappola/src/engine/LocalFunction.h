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
#ifndef _LOCALFUNCTION_H_
#define _LOCALFUNCTION_H_

#include "FunctionWrapper.h"

///
/// Representation of function in the current process.
///
class NktLocalFunction : public NktFunctionWrapper
{
public:
	NktLocalFunction();
	NktLocalFunction(const NktFunctionWrapper&, LPCVOID pThis = NULL);
	NktLocalFunction(const NktLocalFunction&);
	NktLocalFunction& operator= (const NktLocalFunction&);

	/**
	Call the local function.
	@pmsSize size of params in bytes.
	*/
	void Call(NktRegisters*, LPCVOID params = NULL, USHORT pmsSize = 0);
	void Call(LPCVOID params = NULL, USHORT pmsSize = 0);

protected:
	LPCVOID _pThis;
};

#endif //_LOCALFUNCTION_H_