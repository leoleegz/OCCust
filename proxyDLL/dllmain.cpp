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
       
#include "msimgproxy.h"
#include <tchar.h>
#include <strsafe.h>


PFNTRANSPARENTBLT	pfnTransparentBlt	= NULL;
PFNALPHABLEND		pfnAlphaBlend		= NULL;
PFNDLLINITIALIZE	pfnDllInitialize	= NULL;
PFNGRADIENTFILL		pfnGradientFill		= NULL;
PFNVSETDDRAWFLAG	pfnVSetDdrawFlag	= NULL;
PFNINITDLL			pfnInitDll			= NULL;

bool		GetMsimg32FnAddr();
HMODULE		hMsimg32 = NULL;
HMODULE		hPlugDll = NULL;

TCHAR pszInjDllName[] = TEXT("BECC\\OCPlugin.dll");

/************************************************************************/
/*																		*/
/* Proxy DLL Entry point                                                */
/*																		*/
/************************************************************************/

BOOL APIENTRY DllMain( HMODULE hModule, DWORD ul_reason_for_call, 
					  LPVOID lpReserved )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:		
		{	
			if (!GetMsimg32FnAddr())
			{
				OutputDebugString (TEXT("MSIMG32 Proxy: GetMsimg32FnAddr() failed\n"));
				//return FALSE;
			}

			OutputDebugString(TEXT("MSIMG32 Proxy: DLL attached\n"));
			
			// Load injected DLL and execute it's initialization function			
			if (! (hPlugDll = LoadLibrary(pszInjDllName)))
			{
				OutputDebugString (TEXT("MSIMG32 Proxy: Cannot load inject-DLL\n"));
				return FALSE;
			}

			if (! (pfnInitDll = (PFNINITDLL) GetProcAddress (hPlugDll, "InitDLL")))
			{
				OutputDebugString (TEXT("MSIMG32 Proxy: No valid address for executing InitDLL\n"));
				return FALSE;
			}

			(*pfnInitDll)();

			break;
		}

	case DLL_PROCESS_DETACH:
		{
			if (hMsimg32)
				if (FreeLibrary(hMsimg32))
					OutputDebugString (TEXT("MSIMG32 Proxy: DLL detached.\n"));
			break;
		}
	}
	return TRUE;
}

void ErrorExit(LPTSTR lpszFunction) 
{ 
    // Retrieve the system error message for the last-error code

    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError(); 

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    // Display the error message and exit the process

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
        (lstrlen((LPCTSTR)lpMsgBuf)+lstrlen((LPCTSTR)lpszFunction)+40)*sizeof(TCHAR)); 
    StringCchPrintf((LPTSTR)lpDisplayBuf, 
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s failed with error %d: %s"), 
        lpszFunction, dw, lpMsgBuf); 
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK); 

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
}


// 
// Get original MSIMG32.DLL functions virtual addresses
//////////////////////////////////////////////////////////////////////////
bool GetMsimg32FnAddr()
{
	TCHAR libName[MAX_PATH] = {0};
	GetSystemDirectory (libName, MAX_PATH);
	//_tcscat_s (libName, MAX_PATH*sizeof(TCHAR), TEXT("\\msimg32.dll\0"));
	_tcscat(libName, TEXT("\\msimg32.dll\0"));
	hMsimg32 = LoadLibrary (libName);

	if (!hMsimg32)
	{
		return false;
	}
	
	if ( (pfnVSetDdrawFlag = (PFNVSETDDRAWFLAG) GetProcAddress(hMsimg32, "vSetDdrawflag")) == NULL)
		return false;

	if ( (pfnAlphaBlend = (PFNALPHABLEND) GetProcAddress(hMsimg32, "AlphaBlend")) == NULL)
		return false;

	if ( (pfnDllInitialize = (PFNDLLINITIALIZE) GetProcAddress(hMsimg32, "DllInitialize")) == NULL)
		return false;

	if ( (pfnGradientFill = (PFNGRADIENTFILL) GetProcAddress(hMsimg32, "GradientFill")) == NULL)
		return false;

	if ( (pfnTransparentBlt = (PFNTRANSPARENTBLT) GetProcAddress(hMsimg32, "TransparentBlt")) == NULL)
		return false;

	return true;
}

//
// Exported function definition
//////////////////////////////////////////////////////////////////////////
BOOL WINAPI TransparentBlt(HDC p1, int p2, int p3, int p4, int p5, HDC p6, int p7, int p8, 
								   int p9, int p10, UINT p11)
{	
	return pfnTransparentBlt (p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11);
}

BOOL WINAPI AlphaBlend(HDC p1, int p2, int p3, int p4, int p5, HDC p6, int p7 , int p8,
							   int p9, int p10, BLENDFUNCTION dw)
{
	return pfnAlphaBlend (p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,dw);
}

BOOL WINAPI GradientFill (HDC p1, PTRIVERTEX p2, ULONG p3, PVOID p4, ULONG p5, ULONG p6)
{
	return pfnGradientFill (p1, p2, p3, p4, p5, p6);
}

BOOL WINAPI DllInitialize (HINSTANCE d1,DWORD d2, LPVOID d3)
{
	return pfnDllInitialize (d1, d2, d3);
}

VOID WINAPI vSetDdrawflag (VOID) 
{
	(*pfnVSetDdrawFlag)(); 
}
