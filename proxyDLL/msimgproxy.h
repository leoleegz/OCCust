/*****************************************************************************
Windows Live Messenger Plugin Demo -- Proxy DLL 
Copyright (C) 2008  Hernán Di Pietro

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

/*****************************************************************************/
#ifndef MSIMGPROXY_H
#define MSIMGPROXY_H
#include <windows.h>
#pragma warning( disable:4273 ) // Silence compiler

//
// Function pointer typedefs for exports we are forwarding              
//
typedef BOOL (WINAPI *PFNTRANSPARENTBLT) (HDC,int,int,int,int,HDC,int,int,int,int,UINT);
typedef VOID (WINAPI *PFNVSETDDRAWFLAG)  (VOID);
typedef BOOL (WINAPI *PFNALPHABLEND)	(HDC,int,int,int,int,HDC,int,int,int,int,BLENDFUNCTION);
typedef BOOL (WINAPI *PFNGRADIENTFILL)	(HDC,PTRIVERTEX,ULONG,PVOID,ULONG,ULONG);
typedef BOOL (WINAPI *PFNDLLINITIALIZE)(HINSTANCE, DWORD, LPVOID);

// Function pointer typedef to injected-DLL initialization
typedef void (*PFNINITDLL) (void);	

#ifdef __cplusplus
extern "C" 
{
#endif 
	//
	// Function protoypes
	//
	BOOL WINAPI TransparentBlt(HDC, int, int, int, int, HDC, int, int, int, int, UINT);
	BOOL WINAPI AlphaBlend    (HDC, int , int, int, int, HDC, int , int, int, int, BLENDFUNCTION);
	BOOL WINAPI GradientFill  (HDC, PTRIVERTEX, ULONG, PVOID, ULONG, ULONG);
	BOOL WINAPI DllInitialize (HINSTANCE, DWORD, LPVOID);
	VOID WINAPI vSetDdrawflag (VOID);

#ifdef __cplusplus
}
#endif // __CPLUSPLUS
#endif // MSIMGPROXY_H

