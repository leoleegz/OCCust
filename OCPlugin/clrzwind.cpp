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

#include "clrzwind.h"
#include "OCPlugin.h"

ATOM RegisterColorizedWndClass()
{
	WNDCLASSEX wcex;
	ZeroMemory(&wcex, sizeof(WNDCLASSEX));

	wcex.lpszClassName  = wszColorizedWndClass;
	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hInstance		= hDllInst;
	wcex.lpfnWndProc	= (WNDPROC)ColorizedWndProc;
	wcex.style			= CS_DROPSHADOW;
	wcex.cbClsExtra		= sizeof(COLORREF);		// Global base color for class

	return RegisterClassEx(&wcex);
}

// Returns the stored color base from the window class
//
COLORREF GetClrzBaseColor(HWND hwnd)
{
	return (COLORREF) GetClassLongPtr(hwnd, 0);
}

// Sets the base color for a colorized window class
//
void SetClrzBaseColor (HWND hwnd, COLORREF crBase)
{
	SetClassLongPtr (hwnd, 0, (LONG_PTR) crBase);
}

// Window procedure for colorized windows
// Specific processing must be done through subclassing.
//
LRESULT ColorizedWndProc (HWND hwnd, UINT uMsg, WPARAM wparam, LPARAM lparam)
{
	COLORREF crBase;
	static HRGN hRegion = NULL;
	RECT wndRect;
	HDC hdc, hdcClient;
	UINT uHitTest;

	switch (uMsg)
	{
	case WM_CREATE:
		{
			POINT pt;

			// Set rounded-rect shape for the window
			GetClientRect(hwnd, &wndRect);
			pt.x = wndRect.right;
			pt.y = wndRect.bottom;
			ClientToScreen(hwnd, &pt);
			hRegion = CreateRoundRectRgn(0, 0, pt.x, pt.y, CX_RR_ELLIPSE, CY_RR_ELLIPSE);
			SetWindowRgn(hwnd, hRegion, TRUE);
			return 0L;
		}

	case WM_ERASEBKGND:		
		// Fill gradient background and draw frame around the window region

		hdc = (HDC)wparam;
		crBase = GetClrzBaseColor(hwnd);
		GetClientRect(hwnd, &wndRect);

		TRIVERTEX        vert[2];
		GRADIENT_RECT    gRect;
		vert [0] .x      = 0;
		vert [0] .y      = 0;
		vert [0] .Red    = GetRValue(crBase)<<8;
		vert [0] .Green  = GetGValue(crBase)<<8;
		vert [0] .Blue   = GetBValue(crBase)<<8;
		vert [0] .Alpha  = 0x0000;

		vert [1] .x      = wndRect.right;
		vert [1] .y      = wndRect.bottom;
		vert [1] .Red    = max( 0, (GetRValue(crBase)<<8) - 0x4000);
		vert [1] .Green  = max( 0, (GetGValue(crBase)<<8) - 0x4000);
		vert [1] .Blue   = max( 0, (GetBValue(crBase)<<8) - 0x4000);
		vert [1] .Alpha  = 0x0000;

		gRect.UpperLeft  = 0; 
		gRect.LowerRight = 1;

		GradientFill(hdc, vert, 2, &gRect, 1, GRADIENT_FILL_RECT_V);	

		// Draw WLM-like border
		SelectObject(hdc,GetStockObject(DC_PEN));
		SetDCPenColor(hdc, CR_OUTERBORDERCOLOR);
		SelectObject(hdc,GetStockObject(HOLLOW_BRUSH));
		RoundRect(hdc, 0,0, wndRect.right - 1, wndRect.bottom - 1, 
			CX_RR_ELLIPSE, CY_RR_ELLIPSE );
		SetDCPenColor(hdc, CR_INNERBORDERCOLOR(crBase) );
		RoundRect(hdc, 1, 1, wndRect.right - 2, wndRect.bottom - 2, 
			CX_RR_ELLIPSE, CY_RR_ELLIPSE);

		return 1L;

	case WM_DESTROY:
		if (hRegion)
			DeleteObject(hRegion);
		PostQuitMessage(0);
		return 0L;

	case WM_UPDATEBASECOLOR:
		// Sets the base color from the WLM window and repaints itself
		// Get a pixel color at (50,1) from the WLM client area	border
		hdcClient = GetDCEx (g_ocTopWindow.hwnd, 0, DCX_CACHE);	
		SetClrzBaseColor(hwnd, GetPixel (hdcClient, COLORSAMPLE_X, COLORSAMPLE_Y));		
		ReleaseDC(g_ocTopWindow.hwnd, hdcClient);	
		InvalidateRect(hwnd, NULL, TRUE);
		return 0L;

	case WM_NCHITTEST:
		uHitTest = DefWindowProc(hwnd, WM_NCHITTEST, wparam, lparam);
		if(uHitTest == HTCLIENT)
			return HTCAPTION;
		else
			return uHitTest;

	default:
		return DefWindowProc (hwnd, uMsg, wparam, lparam);
	}	
	return 0L;
}

