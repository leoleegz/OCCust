// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <list>
#include <map>
#include <Richedit.h>
#include <unknwn.h>
#include <Textserv.h>
#include "utility.h"
#include "hooking.h"
#include "resmgr.h"
#include "acdisp.h"

// Messenger API Typelib
#import "../tlb/Communicator.tlb" named_guids

// Global instance handle for the injected DLL
extern HINSTANCE hDllInst;

// Resource DLL name
const WCHAR wszResourceDll[] = L"beccres.dll";

// Exports
#ifdef __cplusplus
extern "C"
{
#endif

void InitDLL (void);

#ifdef __cplusplus
}
#endif // __cplusplus

// ---------------------------------------------------------------------------
// Declarations for WLM 
// ---------------------------------------------------------------------------

const WCHAR wszOCMainWndClass[] = L"CommunicatorMainWindowClass";
const WCHAR wszOCMainWndName[] = L"Office Communicator";
const WCHAR wszOCDUIWndClass[] = L"DirectUIHWND";

// WLM top level window struct
typedef struct 
{
	const WCHAR*	szWndClass;
	const WCHAR*	szWndName;	
	WNDPROC	pfnOldWndProc;
	HWND	hwnd;
	bool	fCreated;
	BOOL	fEnableInfoWnd;
	HWINEVENTHOOK hwevh;
} OC_TOPWINDOW;

// WLM "DirectUI" Window 
typedef struct
{
	const WCHAR*	szWndClass;
	HWND	hwnd;
} OC_DIRECTUIWINDOW;

// Contact Info Window
typedef struct 
{
	HWND	hwnd;
	LONG	lTop, lLeft;
	COLORREF crBase;
	WCHAR*	wszContactID;
	WCHAR*	wszFriendlyName;
	CommunicatorAPI::MISTATUS msStatus;
	VARIANT_BOOL fBlocked;
	VARIANT_BOOL fCanPage;
	WCHAR*	wszMobilePhone;
	WCHAR*	wszHomePhone;
	WCHAR*	wszWorkPhone;
} OC_CONTACTINFOWINDOW;

// WLM interface pointers
typedef struct
{
	CommunicatorAPI::IMessenger* pIOc;
} OC_IFACES;

// Toolbar button properties structure
typedef struct tagOCTOOLBARBUTTON
{
	BOOL		fExtBitmap;		// TRUE if bitmap comes from external DLL,
								// FALSE if bitmap comes from WLM Res Type #4000
	HMODULE		hModule;		// Module if bitmap loaded from resource DLL
	WCHAR		wszResType[MAXSTRL];		// Resource type in resource DLL (e.g "PNG")
	UINT		uResID;			// Resource ID from resource DLL, or image ID from 
								// WLM Resource #4000
	UINT		uPosition;		// Insert button in front or back of toolbar
	char		szButtonId[MAXSTRL];		// Unique Button ID
	WCHAR		wszButtonId[MAXSTRL];	// WCHAR string of szButtonID (for action)
	char		szTButtonId[MAXSTRL];	// Unique toolbar button ID (different from szButtonId)
	char		szTooltip[MAXSTRL];		// Tooltip String		
	PFNBTNPROC	pfnAct;		// Pointer to function when button is pressed
} OC_TOOLBARBUTTON;

// Position of added WLM toolbar buttons 
#define TB_INSERT_FIRST 0
#define TB_INSERT_LAST	1

#define TB_INSERT_FIRST_BYTE	0x1524
/*#define TB_INSERT_LAST_BYTE		0x1e05*/
#define OCRES_STYLE_INSERT_POS	23


//
// IDs for resource hooking
// "DirectUI" resources (see resids.txt on project root)
//
#define OCRES_XMLUI			4004
#define OCRES_XMLSTYLE			4005
#define OCRES_BITMAPS			4000
#define OCRES_CONTACT_LIST		923

// Standard Win32 Resources
#define ID_MENU_DEMO_ABOUT	60000
#define ID_MENU_MSNHACK_IAC1	60001
#define ID_MENU_DISPLAY_CONTACTINFO 60002

// starting number for new resources
#define RESOURCE_ID_BASE	63000

// forward declarations for globals
extern OC_TOPWINDOW	g_ocTopWindow;
extern OC_DIRECTUIWINDOW g_ocDUIWindow;
extern OC_CONTACTINFOWINDOW g_ocContactInfoWindow;
extern OC_IFACES		g_ocIfaces;
extern HookList		    g_hookList;
extern CResourceManager	g_resMgr;
extern CActionDispatcher g_actDisp;
extern HMODULE			hResLib;


typedef HRESULT (WINAPI *PCREATETEXTSERVICES)(IUnknown*, ITextHost*, IUnknown**);

//
// function prototypes
//

LRESULT CALLBACK OcWndProc(HWND, UINT, WPARAM, LPARAM);
void CALLBACK HandleWinEvent(HWINEVENTHOOK, DWORD , HWND, LONG, LONG, DWORD, DWORD );
void AddToolbarButtons (OC_TOOLBARBUTTON*, UINT, char*, char*, size_t, size_t );
bool FindOCMainWindow();