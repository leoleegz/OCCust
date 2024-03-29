/*****************************************************************************
Windows Live Messenger Plugin Demo
Copyright (C) 2008  Hern�n Di Pietro

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
#ifndef UTILITY_H
#define UTILITY_H
#include <windows.h>
#include <crtdbg.h>
#include <cstdio>

// miscellaneous defines
#define	MAXSTRL	256
#define	MAKEULONGLONG(hi,lo)	((ULONGLONG)(0x0ULL | (ULONGLONG)(hi) << 32 | (DWORD)(lo)))

// function prototypes
void _OutputDebugString (LPWSTR , ... );
void StringFromGUID		(const GUID, WCHAR*);

#endif //UTILITY_H
