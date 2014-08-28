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
#include "LocalFunction.h"
#include "Utils.h"

///
/// Constructors / Destructors
///
NktLocalFunction::NktLocalFunction() : _pThis(NULL) {}
NktLocalFunction::NktLocalFunction(const NktFunctionWrapper& fw, LPCVOID pThis)
: NktFunctionWrapper(fw), _pThis(pThis) {}
NktLocalFunction::NktLocalFunction(const NktLocalFunction& lf)
: NktFunctionWrapper(lf), _pThis(lf._pThis) {}
NktLocalFunction& NktLocalFunction::operator=(const NktLocalFunction& cp)
{
	NktFunctionWrapper::operator=(cp);
	_pThis = cp._pThis;
	return *this;
}


///
/// Call
///
void NktLocalFunction::Call(NktRegisters* registers, LPCVOID params, USHORT pmsSize)
{
	NktUtils::CallFunction(*this, registers, params, pmsSize);
}
void NktLocalFunction::Call(LPCVOID params, USHORT pmsSize)
{
	NktUtils::SimpleCallFunction(*this, params, pmsSize, _pThis);
}