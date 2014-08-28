#include "RecvWin.h"
#include "StdAfx.h"
#include "RichEd20.h"

/************************************
  REVISION LOG ENTRY
  Revision By: Zhang, Zhefu
  E-mail: codetiger@hotmail.com
  Revised on 10/2/2003 
  Comment: This is program code accompanying "COM Interface Hooking and Its Application"
           written by Zhefu Zhang posted on www.codeguru.com 
           You are free to reuse the code on the base of keeping this comment
		   All Right Reserved by author		   
 ************************************/

#define WND_CLASS _T("BECCEDITRECV")
#define WND_TITLE _T("BECCEDITRECV")

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= 0; //CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL; //LoadIcon(hInstance, (LPCTSTR)IDI_DSA);
	wcex.hCursor		= NULL; //LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= NULL; //(HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL; // (LPCSTR)IDC_DSA;
	wcex.lpszClassName	= WND_CLASS; //szWindowClass;
	wcex.hIconSm		= NULL; //LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}


HWND InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   return CreateWindow(WND_CLASS, WND_TITLE, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message) 
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_TIMER:
            ::QueryChatContent(0); //query the first chat only for now
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

