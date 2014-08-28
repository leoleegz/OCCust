#ifndef _FUNCTIONWRAPPER_H_
#define _FUNCTIONWRAPPER_H_

#include "types.h"

/**
Class representing a Function.
*/
class NktFunctionWrapper
{
public:
	NktFunctionWrapper();
	NktFunctionWrapper(const void* fnc, int cc = cdecl_, USHORT pmSize = 0);
	NktFunctionWrapper(const NktFunctionWrapper&);
	NktFunctionWrapper& operator=(const NktFunctionWrapper&);

	/**
	Function Address.
	*/
	const void* GetAddress() const;

	/**
	Calling Convention.
	*/
	int GetCallConvention() const;

	/**
	Full size of params.
	*/
	USHORT GetParamSize() const;

	/**
	Size of params in stack.
	*/
	USHORT GetStackParamSize() const;

	/**
	Full size of required stack clean.
	*/
	USHORT GetCallerCleanSize() const;
	USHORT GetCalleeCleanSize() const;

	/**
	Test if function is a valid one.
	*/
	virtual bool isValid() const;

protected:
	NktCallingConvention _cc;
	const void* _fnc;
	USHORT _pmSize;
};

#endif //_FUNCTIONWRAPPER_H_
