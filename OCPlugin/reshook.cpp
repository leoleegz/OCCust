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
void Handle_FindResourceW (NktHandlerParams* hp)
{
	// Note that WLM seems to use integer resources

	HMODULE hMod = *(HMODULE*) PARAMETER_INDEX(0);
	DWORD dwName = (DWORD)(*(LPCTSTR*)PARAMETER_INDEX(1));
	DWORD dwType = (DWORD)(*(LPCTSTR*)PARAMETER_INDEX(2));
	ULONGLONG ulid = MAKEULONGLONG(dwType,dwName);

	//_OutputDebugString(L"Handle_FindResourceW: dwName %d, dwType %d",dwName,dwType);

	// find out if this resource is registered to be hooked
	if (g_resMgr.IsResourceRegistered(dwType,dwName))
	{
		//OutputDebugString(L"Resource is registered.");

		// add a handle-table entry for the resource

		if (g_resMgr.IsNewResource(ulid)) 
		{
			// For adding a new resource to WLM from the resource DLL
			// we simply return the handle to it		

			g_resMgr.AddHandleTableEntry(ulid, g_resMgr.GetDLLResourceHandle(ulid));
			g_resMgr.SetResourceSize(ulid, g_resMgr.GetDLLResourceSize(ulid));
			hp->iHook->SetReturnValue(&hp->context,(INT_PTR) g_resMgr.GetDLLResourceHandle(ulid));
		}
		else
		{
			if ((HRSRC)WINAPI_RETVAL)
			{
				g_resMgr.AddHandleTableEntry (ulid,(HRSRC)WINAPI_RETVAL);

				// save the resource size for later use 
				//_OutputDebugString(L"Handle_FindResourceW: Calling SizeOfresource for handle %d", (HRSRC)WINAPI_RETVAL);
				DWORD dwSize = SizeofResource(hMod, (HRSRC)WINAPI_RETVAL);
				//_OutputDebugString(L"Handle_FindResourceW: SizeOfResource returned %d bytes, LastError %d", dwSize, GetLastError());
				g_resMgr.SetResourceSize (MAKEULONGLONG(dwType,dwName), dwSize);	
			}
		}
	}
}

// ---------------------------------------------------------------------------
void Handle_LoadResource (NktHandlerParams* hp)
{
	HRSRC hRes = *(HRSRC*)PARAMETER_INDEX(1);
	_OutputDebugString(L"Handle_LoadResource %d", hRes);

	// check if this handle is in the registered handle table
	if (g_resMgr.IsHandleRegistered(hRes))
	{	

		ULONGLONG res = g_resMgr.ResourceIDFromHandle(hRes);
		if (g_resMgr.IsNewResource(res))
		{
			// return the stored handle for DLL resources

			g_resMgr.AddPointerTableEntry (res, g_resMgr.GetDLLResourceDataHandle(res));
			hp->iHook->SetReturnValue(&hp->context,(INT_PTR)g_resMgr.GetDLLResourceDataHandle(res));
		}
		else
		{
			// add an entry with the handle to resource data, and register
			// to which resource it's related (leave the resource address for LockResource)

			g_resMgr.AddPointerTableEntry (res, (HGLOBAL)WINAPI_RETVAL);			
		}		
	}	
}

// ---------------------------------------------------------------------------
void Handle_LockResource (NktHandlerParams* hp)
{
	HGLOBAL hpData = *(HGLOBAL*)PARAMETER_INDEX(0);
	if ((DWORD)WINAPI_RETVAL)
	{
		// check if data pointer handle is registered
		if (g_resMgr.IsDataPointerRegistered(hpData))
		{
			// Here we operate with the resource if we are requesting
			// to be modified (or copied to the new virtual address).
			// Set the pointer data depending on the requested operation
			// on the resource. 

			g_resMgr.SetResourcePointer (hpData, (LPVOID) WINAPI_RETVAL);
			LPVOID lpvResourceAddress = g_resMgr.AllocResource(hpData);
			if (lpvResourceAddress)
			{
				g_resMgr.SetResourcePointer (hpData, lpvResourceAddress);		
				g_resMgr.DumpTables();
				hp->iHook->SetReturnValue(&hp->context, (INT_PTR) lpvResourceAddress);
			}
		}		
	}	
}


// ---------------------------------------------------------------------------
void Handle_SizeofResource (NktHandlerParams* hp)
{
	DWORD hMod = (DWORD) (*(HMODULE*) PARAMETER_INDEX(0));
	HRSRC hr   = *(HRSRC*)PARAMETER_INDEX(1);

	if (g_resMgr.IsHandleRegistered(hr))
	{
		_OutputDebugString (L"Handle_SizeOfResource: handled with hModule=%d", hMod ); 

		// if this is the first time we call, return original resource size
		if (g_resMgr.GetOriginalResourceSize(hr) != NULL)
		{	
			_OutputDebugString (L"Handle_SizeOfResource: First 'hooked' call for resource %d ",hr);
			hp->iHook->SetReturnValue (&hp->context, g_resMgr.GetFixedResourceSize (hr));			
		}
		else
			OutputDebugString (L"Handle_SizeOfResource: cbOldSize is NULL, return with default value");

		//g_resMgr.DumpTables();
	}	
	//else
	//	_OutputDebugString (L"Handle_SizeOfResource with unregistered res handle %d\n\n",hr);
}
