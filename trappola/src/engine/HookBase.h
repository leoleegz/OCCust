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
#ifndef _HOOKBASE_H_
#define _HOOKBASE_H_

#include "FunctionWrapper.h"
#include "VirtualCode.h"
#include "LocalFunction.h"
#include "HookTypes.h"

/**
Base class for local hooks.
*/
class NktHookBase : public INktHookSimple
{
public:
	NktHookBase();
	~NktHookBase();

	/**
	Call hooked function with specified context.
	*/
	void CallFunction(NktHookCallContext*);

	/**
	Skip call and and use retVal on return.
	*/
	void SkipCall(NktHookCallContext*, INT_PTR retVal);

	/**
	Global hooks enable:
	*/
	static BOOL GetGlobalEnabled();
	static void SetGlobalEnabled(BOOL enable);

	/**
	Force return value in call.
	*/
	void SetReturnValue(NktHookCallContext*, INT_PTR retVal);

	/**
	Force lastError value in call.
	*/
	void SetLastError(NktHookCallContext*, INT_PTR err);

	/**
	Disable reports from calling thread.
	*/
	static HANDLE DisableThreadReports();

	/**
	Restore previous state of thread reports.
	*/
	static void RestoreThreadReports(HANDLE);

	/**
	Tells if reports are disabled for the calling thread.
	*/
	static BOOL IsThreadDisabled();

	/**
	Enable / Disable reports
	*/
	void SetEnabled(BOOL);
	BOOL GetEnabled() const;

	/**
	Get hook flags
	*/
	int GetFlags() const;

protected:
	/**
	Implement call of hooked function.
	*/
	virtual void RawCallFunction(NktHookCallContext*) = ABSTRACT;

protected:
	void SetHandler(const NktLocalFunction&);
	const NktFunctionWrapper& GetHandler() const;

	void SetFlags(int);

	void SetUserData(INT_PTR);
	INT_PTR GetUserData() const;

	/**
	Internal: First Handler.
	*/
	void __fastcall InternalHandler(NktRegisters* reg, INT_PTR tag);

private:
	NktLocalFunction _handler;
	int _flags;
	INT_PTR _userData;
	BOOL _disabled;

protected:
	static BOOL hooksDisabled;
};

#endif //_HOOKBASE_H_