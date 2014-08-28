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

// ---------------------------------------------------------------------------
void Handle_CoRegisterClassObject (NktHandlerParams * hp)
{	
	HRESULT			hr;
	WCHAR			wszBuf [256];
	CLSID			clsid = **(CLSID**)PARAMETER_INDEX(0);
	IUnknown*		pUnk = *(IUnknown**) PARAMETER_INDEX(1);
	IClassFactory*	pIcf = NULL;

	if (SUCCEEDED((HRESULT)WINAPI_RETVAL))
	{
		StringFromGUID(clsid, wszBuf);
		_OutputDebugString (L"S_OK, Handling CoRegisterClassObject for CLSID %s", wszBuf);	

		// check CLSID ...
		if (clsid == CommunicatorAPI::CLSID_Messenger)
		{
			OutputDebugString (L"Handling CoRegisterClassObject for CLSID_Messenger\n");
			// get ptr to class factory
			hr = pUnk->QueryInterface(IID_IClassFactory, (void**)&pIcf);
			if (SUCCEEDED(hr))
			{
				hr = pIcf->CreateInstance (NULL, CommunicatorAPI::IID_IMessenger, 
					(void**) &g_ocIfaces.pIOc);

				_ASSERTE(hr == S_OK);				
			}			
			pUnk->Release();

			// attach hook to handle MSAA events
			g_ocTopWindow.hwevh = SetWinEventHook(EVENT_OBJECT_FOCUS, 
				EVENT_OBJECT_STATECHANGE, GetModuleHandle(0), &HandleWinEvent, GetCurrentProcessId(), NULL, 
				WINEVENT_INCONTEXT);
		}
	} 
}

//-------------------------------------------------------------------------------------------