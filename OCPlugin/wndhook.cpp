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
#include "OCPlugin.h"
#include "Oleacc.h"
#include "initguid.h"
#include "clrzwind.h"

// ---------------------------------------------------------------------------
// If an accessibility object contains a name that matches the WLM contact
// format (we assume ends with < mail_address >) it returns the contact ID
// (actually *this is* the email address)
//
// returns FALSE if the source string does not conform 
// 
static BOOL AccObjToContactID(const WCHAR* wszAccName, WCHAR* wszContactId)
{
	int cAtChars = 0;		// number of @ chars
	const WCHAR* wszBegin = wszAccName;	

	if (wszAccName == NULL) return FALSE;

	wszAccName += wcslen(wszAccName)-1;  // advance towards end
	if (*wszAccName == '>')
	{
		// search backwards until '<', count @
		while (*wszAccName-- != '<' && wszBegin != wszAccName )
			if (*wszAccName == '@') cAtChars++;
		
		// invalid -- this is not an accessibility object for a contact
		if (wszAccName == wszBegin || cAtChars > 1)
			return FALSE;
		else
		{
			// copy ID without < >
			wcscpy_s (wszContactId, MAXSTRL, wszAccName+2);
			wszContactId[wcslen(wszContactId)-1] = '\0';
			return TRUE;
		}
	}
	else
		return FALSE;
};

// Callback function that handles MS Accessibility events
//
void CALLBACK HandleWinEvent(HWINEVENTHOOK hook, DWORD event, HWND hwnd, 
							 LONG idObject, LONG idChild, 
							 DWORD dwEventThread, DWORD dwmsEventTime)
{
	IAccessible* pIAcc = 0;
	CommunicatorAPI::IMessengerContact* pIContact = 0;
	VARIANT varChild, varRole, varState;
	BSTR name = NULL;
	WCHAR wszID[MAXSTRL];

	VariantInit(&varChild);
	HRESULT hr = AccessibleObjectFromEvent (hwnd, idObject, idChild, &pIAcc, &varChild);

	if ((hr == S_OK) && (pIAcc != NULL)) 
	{
		if (pIAcc->get_accName(varChild, &name) != S_FALSE)
		{
			switch (event)
			{
			case EVENT_OBJECT_FOCUS:
				// process only if the contact info window menu option is enabled

				if (AccObjToContactID(name, wszID) && g_ocTopWindow.fEnableInfoWnd)
				{	
					BSTR bstrID = SysAllocString(wszID);					
					IDispatch* pIDisp;
					hr = g_ocIfaces.pIOc->raw_GetContact(bstrID, g_ocIfaces.pIOc->MyServiceId, &pIDisp);
					
					if (SUCCEEDED(hr))
					{
						if (SUCCEEDED(pIDisp->QueryInterface(CommunicatorAPI::IID_IMessengerContact, 
							(void**)&pIContact)))
						{
							//ShowWindow(g_ocContactInfoWindow.hwnd, SW_SHOWNA);

							//// force window to repaint with new values
							//g_ocContactInfoWindow.wszContactID = bstrID;
							//g_ocContactInfoWindow.wszFriendlyName = pIContact->FriendlyName;
							//g_ocContactInfoWindow.msStatus = pIContact->Status;
							//g_ocContactInfoWindow.fBlocked = pIContact->Blocked;
							//g_ocContactInfoWindow.fCanPage = pIContact->CanPage;
							//g_ocContactInfoWindow.wszHomePhone 
							//	= pIContact->GetPhoneNumber(MSNMessenger::MPHONE_TYPE_HOME);
							//g_ocContactInfoWindow.wszWorkPhone
							//	= pIContact->GetPhoneNumber(MSNMessenger::MPHONE_TYPE_WORK);
							//g_ocContactInfoWindow.wszMobilePhone
							//	= pIContact->GetPhoneNumber(MSNMessenger::MPHONE_TYPE_MOBILE);

							//SendMessage(g_ocContactInfoWindow.hwnd, WM_UPDATEBASECOLOR, 0, 0);
							pIContact->Release();
						}
						pIDisp->Release();
					}
					SysFreeString(bstrID);
				}	
				else // if it's focused to a non-contact element...
				{
					// Hide contact info window
					ShowWindow(g_ocContactInfoWindow.hwnd, SW_HIDE);
				}
				break;

			case EVENT_OBJECT_STATECHANGE:
				// We handle this for toolbar buttons going to "pressed" state
				varRole.vt = VT_I4;
				pIAcc->get_accRole(varChild, &varRole);
				pIAcc->get_accState(varChild, &varState);
				if (varRole.lVal == ROLE_SYSTEM_BUTTONMENU)
				{			
					if (varState.lVal == STATE_SYSTEM_PRESSED)
					// Execute action related to this button name
					g_actDisp.ExecuteAction(std::wstring(name), NULL);
				}
				break;
			}		

			SysFreeString(name);
		}
		pIAcc->Release();
	}	
	VariantClear(&varChild);	
}

void Handle_CreateWindowExW (NktHandlerParams * hp)
{
	// Check if it's creating WLM top level window
	// Warning! window class parameter can be interpreted as a pointer-to-string 
	// or as an atom. Atoms high-word are always zero

	LPCWSTR lpszWndClass = *(LPCWSTR*) PARAMETER_INDEX(1);
	if (lpszWndClass && HIWORD((INT_PTR )lpszWndClass) )
		if (wcscmp(lpszWndClass, g_ocTopWindow.szWndClass) == 0)
		{
			g_ocTopWindow.hwnd = (HWND)hp->context.regs->EAX;

			// Subclass window to process messages the way we want...
			g_ocTopWindow.pfnOldWndProc = (WNDPROC) GetWindowLongPtr (g_ocTopWindow.hwnd, GWLP_WNDPROC);
			SetWindowLongPtr (g_ocTopWindow.hwnd, GWLP_WNDPROC, (LONG_PTR)OcWndProc);					
		}
}

bool FindOCMainWindow()
{
	HWND hMainWindow = FindWindow(wszOCMainWndClass, NULL);
	if( hMainWindow == NULL )
		return false;

	g_ocTopWindow.hwnd = hMainWindow;
	g_ocTopWindow.pfnOldWndProc = (WNDPROC) GetWindowLongPtr (g_ocTopWindow.hwnd, GWLP_WNDPROC);
	SetWindowLongPtr (g_ocTopWindow.hwnd, GWLP_WNDPROC, (LONG_PTR)OcWndProc);	

	return true;
}

// ---------------------------------------------------------------------------
// sub classed top-level window procedure
LRESULT CALLBACK OcWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HMENU hMainMenu;
	static HMENU hDemoMenu1;
	RECT r;
	bool fDone  = false;
	
	switch (uMsg)
	{
	case WM_SHOWWINDOW:

		hMainMenu = GetMenu(hwnd);
		hDemoMenu1 = CreatePopupMenu();

		AppendMenu(hMainMenu, MF_STRING | MF_POPUP, (UINT_PTR) hDemoMenu1, L"W&lmPluginDLL");
		AppendMenu(hDemoMenu1, MF_STRING, ID_MENU_DISPLAY_CONTACTINFO, L"Display Contact Information &Window");
		AppendMenu(hDemoMenu1, MF_STRING, ID_MENU_DEMO_ABOUT, L"&About...");
		DrawMenuBar(hwnd);		

	case WM_COMMAND:
		// check if it's from our menu
		if (HIWORD(wParam) == 0)
		{
			switch (LOWORD(wParam))
			{
			//case ID_MENU_DEMO_ABOUT:
			//	{
			//		SendMessage(g_hwndNktAbout, WM_UPDATEBASECOLOR, 0, 0);
			//		SendMessage(g_hwndNktAbout, WM_CREATECHILDCTRLS, 0, 0);
			//		GetWindowRect(g_hwndNktAbout, &r);

			//		SetWindowPos(g_hwndNktAbout, HWND_TOP,  (GetSystemMetrics(SM_CXSCREEN) / 2) - (r.right/2),
			//			(GetSystemMetrics(SM_CYSCREEN) / 2) - (r.bottom/2) , 0, 0, SWP_NOSIZE);				
			//		AnimateWindow(g_hwndNktAbout, 300, AW_BLEND | AW_ACTIVATE);		
			//		InvalidateRect(g_hwndNktAbout, NULL, FALSE); 
			//		break;
			//	}

			//case ID_MENU_DISPLAY_CONTACTINFO:
			//	g_wlmTopWindow.fEnableInfoWnd = ~g_wlmTopWindow.fEnableInfoWnd;
			//	CheckMenuItem(hDemoMenu1, ID_MENU_DISPLAY_CONTACTINFO, 
			//		g_wlmTopWindow.fEnableInfoWnd ? MF_CHECKED : MF_UNCHECKED);
			//	break;
			}
		}	
		break;
		
	case WM_MOVE:
		// change the contact info window position along this (if it's present)

		//RECT r;	
		//POINT ptTopLeft;		
		//GetClientRect(hwnd, &r);
		//ptTopLeft.x = r.left;
		//ptTopLeft.y = r.top;
		//ClientToScreen(hwnd, &ptTopLeft);
		//
		//MoveWindow (g_wlmContactInfoWindow.hwnd, ptTopLeft.x - CIW_DEFAULT_WIDTH - 10, 
		//	ptTopLeft.y, CIW_DEFAULT_WIDTH, CIW_DEFAULT_HEIGHT, TRUE);

		break;

	}
	return CallWindowProc(g_ocTopWindow.pfnOldWndProc, hwnd, uMsg, wParam, lParam);		
}
