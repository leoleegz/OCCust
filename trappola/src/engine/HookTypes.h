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
#ifndef _HOOKTYPES_H_
#define _HOOKTYPES_H_

#include "yasper.h"
#include "types.h"

/**
Hook context state
*/
typedef enum _NktHookContextState
{
	_ctx_none = 0,
	_ctx_called = 1,
	_ctx_skip_call = 2
} NktHookContextState;

/**
Hooked Function Call Context
*/
typedef struct _NktHookCallContext : NktCallContext
{
	int state; //NktHookContextState
} NktHookCallContext;

/**
Hook handler's params
*/
class INktHookSimple;
typedef struct
{
	int flags;
	NktHookCallContext context;
	LPCVOID teb;
	INT_PTR userData;
	INT_PTR callCookie;
	INT_PTR handlerData;
	INktHookSimple* iHook;
} NktHandlerParams;

/**
Hook handler function definition
*/
typedef void (*Handler)(NktHandlerParams*);

/**
Common IHook Interface.
*/
class INktHookSimple
{
public:
	virtual ~INktHookSimple() {};

	/**
	Call hooked function with specified context.
	*/
	virtual void CallFunction(NktHookCallContext*) = ABSTRACT;

	/**
	Skip call and and use retVal on return.
	*/
	virtual void SkipCall(NktHookCallContext*, INT_PTR retVal) = ABSTRACT;

	/**
	Force return value in call.
	*/
	virtual void SetReturnValue(NktHookCallContext*, INT_PTR retVal) = ABSTRACT;

	/**
	Force lastError value in call.
	*/
	virtual void SetLastError(NktHookCallContext*, INT_PTR err) = ABSTRACT;

	/**
	Are reports enabled
	*/
	virtual BOOL GetEnabled() const = ABSTRACT;

	/**
	Enable reports
	*/
	virtual void SetEnabled(BOOL) = ABSTRACT;
};
typedef yasper::ptr<INktHookSimple> IHookSimplePtr;

#endif //_HOOKTYPES_H_