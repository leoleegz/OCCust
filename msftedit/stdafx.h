// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <tchar.h>


// TODO: reference additional headers your program requires here
void PopMsg(LPCTSTR pszFormat, ...) ;
void ReportErr(LPCTSTR str);
void ReportErrEx(LPCTSTR pszFormat, ...) ;

//Always pass len>255 char array here, I am lazy to check boundary
void CreateFileName(LPTSTR szNewFilename, LPCTSTR szPrefix, LPCTSTR szSuffix);
