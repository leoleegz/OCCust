/*****************************************************************************
Windows Live Messenger Plugin Demo
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
#ifndef CLRZWIND_H
#define CLRZWIND_H

#include <windows.h>

// Message to update base color
//
#define	WM_UPDATEBASECOLOR	WM_USER+1

// Colorized window class name
//
const WCHAR wszColorizedWndClass[] = L"NKT_COLORIZED_WINDOW_CLASS";

// Interface elements constants
//
const UINT CX_RR_ELLIPSE = 14;
const UINT CY_RR_ELLIPSE = CX_RR_ELLIPSE;
const UINT CX_TEXT_PADDING = 10;
const COLORREF CR_OUTERBORDERCOLOR = RGB(64,64,64);
inline COLORREF CR_INNERBORDERCOLOR (COLORREF c) 
{
	return RGB (max(255,GetRValue(c)+0x8000),  
		max(255,GetGValue(c)+0x8000),
		max(255,GetBValue(c)+0x8000));
}

// Position of color sample pixel in the WLM client area
//
const UINT COLORSAMPLE_X = 70;
const UINT COLORSAMPLE_Y = 3;

// Function Prototypes
//
ATOM RegisterColorizedWndClass();
COLORREF GetClrzBaseColor(HWND hwnd);
void SetClrzBaseColor (HWND hwnd, COLORREF crBase);
LRESULT ColorizedWndProc (HWND, UINT, WPARAM, LPARAM);
#endif //CLRZWIND_H
