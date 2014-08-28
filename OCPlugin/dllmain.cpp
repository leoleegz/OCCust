// dllmain.cpp : Defines the entry point for the DLL application.
#include "OCPlugin.h"
#include "shellapi.h"
#include "clrzwind.h"
#include "../beccres/resource.h"

HMODULE hResLib = NULL;		// Handle to resource DLL
char * szXmlRes;			// XML Resource string
char * szStyle;				// XML Resource style string
HINSTANCE hDllInst;			// Handle of this DLL

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		OutputDebugString (L"msnhacklib attaching...");
		hDllInst = hModule;
		break;

	case DLL_THREAD_ATTACH:
		break;

	case DLL_THREAD_DETACH:
		break;

	case DLL_PROCESS_DETACH:
		// Shutdown
		OutputDebugString (L"msnhacklib detaching...");
		g_hookList.clear();
		UnhookWinEvent(g_ocTopWindow.hwevh);
		if( g_ocIfaces.pIOc != NULL )
			g_ocIfaces.pIOc->Release();

		CoUninitialize();
		FreeLibrary(hResLib);
		delete[] szStyle;
		delete[] szXmlRes;
		break;
	}
	return TRUE;
}

// ---------------------------------------------------------------------------
// Sets the current directory to WLM directory reading the setting from
// registry key
// ---------------------------------------------------------------------------

void SetCurrentDirToWlmDir()
{
	HKEY hk;
	DWORD bufSize = MAX_PATH;
	BYTE buf[MAX_PATH];

	RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Communicator",0, KEY_READ, &hk);
	RegQueryValueEx(hk, L"InstallationDirectory", NULL, NULL, (BYTE*)buf, &bufSize);
	SetCurrentDirectory((WCHAR*)buf);
	RegCloseKey(hk);
}

// ---------------------------------------------------------------------------
// Initializes the WLM injected library
// ---------------------------------------------------------------------------
void InitDLL()
{
	OutputDebugString (L"InitDLL: Starting up...\n");
	CoInitialize(0);

	// Load Resource DLL
	hResLib = LoadLibrary(wszResourceDll);

	// Set the WLM top level window info
	g_ocTopWindow.szWndClass = wszOCMainWndClass;
	g_ocTopWindow.szWndName  = wszOCMainWndName;	
	g_ocDUIWindow.szWndClass = wszOCDUIWndClass;
	g_ocTopWindow.fEnableInfoWnd = false;

	// Set current directory to WLM dir
	SetCurrentDirToWlmDir();

	// Initialize structures
	SecureZeroMemory(&g_ocIfaces, sizeof(OC_IFACES));	

	// Create the colorized window instances (contact info, about dlg) 
	// after registering the colorized window class
	if (RegisterColorizedWndClass())
	{
		//CreateContactInfoWnd(&g_ocContactInfoWindow.hwnd);
		//CreateAboutDlg (&g_hwndNktAbout);
	}

	// test toolbar buttons

	//OC_TOOLBARBUTTON tbb[2];
	
	//tbb[0].fExtBitmap	= TRUE;
	//tbb[0].hModule		= hResLib;
	//tbb[0].pfnAct		= mplayrun;
	//tbb[0].uPosition	= TB_INSERT_FIRST;
	//tbb[0].uResID		= IDB_MPLAYER;
	//wcscpy_s(tbb[0].wszResType, MAXSTRL, L"PNG");
	//strcpy_s(tbb[0].szButtonId, MAXSTRL, "BUTTONMPLAYER");
	//wcscpy_s(tbb[0].wszButtonId, MAXSTRL, L"BUTTONMPLAYER");
	//strcpy_s(tbb[0].szTButtonId, MAXSTRL, "ToolButtonMplayer");
	//strcpy_s(tbb[0].szTooltip , MAXSTRL, "Run Media Player");

	//tbb[1].fExtBitmap	= TRUE;
	//tbb[1].hModule		= hResLib;
	//tbb[1].pfnAct		= webnkt;
	//tbb[1].uPosition	= TB_INSERT_FIRST;
	//tbb[1].uResID		= IDB_NKT;
	//wcscpy_s(tbb[1].wszResType, MAXSTRL, L"PNG");
	//strcpy_s(tbb[1].szButtonId, MAXSTRL, "BUTTONWEB");
	//wcscpy_s(tbb[1].wszButtonId, MAXSTRL, L"BUTTONWEB");
	//strcpy_s(tbb[1].szTButtonId, MAXSTRL, "ToolButtonMSIE");
	//strcpy_s(tbb[1].szTooltip, MAXSTRL, "Go Trappola Library Website");

	//const size_t cbBufSz = 1024;

	//char * szXmlRes = new char[cbBufSz];
	//char * szStyle  = new char[cbBufSz];
	//AddToolbarButtons(tbb, 2, szXmlRes, szStyle, cbBufSz, cbBufSz);
	if( FindOCMainWindow() )
	{
		// attach our hooks
		AttachHookArray (hookArray, g_hookList); 	
	}
}