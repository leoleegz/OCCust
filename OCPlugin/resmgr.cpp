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
#include "resmgr.h"
#include "utility.h"

CResourceManager::CResourceManager(void) {}
CResourceManager::~CResourceManager(void)
{
	BOOL fvfResult;
	WCHAR msg[MAXSTRL];

	// Free resources
	for (RESOURCE_POINTER_TABLE::iterator it = m_ptrTable.begin();
		it != m_ptrTable.end(); it++)
	{
		wsprintf(msg, L"Freeing allocated resource at address %#x ", it->second.first);
		OutputDebugString (msg);
		fvfResult = VirtualFree (it->second.first, 0, MEM_RELEASE);
		OutputDebugString (fvfResult != 0 ? L"OK" : L"FAIL" ); 
	}
}

// Register a resource to be modified with address and modification type
// (inserting/appending bytes or replacing the entire resource)
//
void CResourceManager::RegisterResource (DWORD dwType, DWORD dwName, LPVOID pResData, DWORD cbSize, 
										 UINT iResModType, UINT cbWhere)
{
	RESOURCE_MOD_DESCRIPTOR rm;
	rm.iModType = iResModType;
	rm.cbSize	= cbSize;
	rm.cbWhere	= cbWhere;
	rm.cbOldSize= NULL;
	rm.pvResData= pResData;
	rm.cbNewSize= NULL;
	rm.pvAddress= NULL;
	rm.fAlloc	= false;		
	m_regResourceTable.insert(RESOURCE_ENTRY(MAKEULONGLONG(dwType,dwName), rm));
}

// Register a resource in the injected DLL
// This is only for res IDs that didn't exist in the original resource 
void CResourceManager::RegisterNewResource (DWORD dwType, DWORD dwName, const RESOURCEINFO& rii)
{
	RESOURCE_MOD_DESCRIPTOR rm;
	rm.iModType = RR_NEW;
	rm.cbSize	= rii.dwSize;
	rm.cbWhere	= NULL;
	rm.cbOldSize= NULL;
	rm.pvResData= rii.pvData;
	rm.cbNewSize= NULL;
	rm.pvAddress= NULL;
	rm.fAlloc	= false;
	rm.ri		= rii;
	m_regResourceTable.insert(RESOURCE_ENTRY(MAKEULONGLONG(dwType, dwName), rm));
}

// Modify a resource identified by the HGLOBAL value returned by LoadResource
// (stored in the resource data pointer table) 
//
LPVOID CResourceManager::AllocResource (const HGLOBAL hDataRes)
{
	// find to which resource it maps
	RESOURCE_POINTER_TABLE::iterator rpit = m_ptrTable.find(hDataRes);
	ULONGLONG resID = rpit->second.second;

	REGISTERED_RESOURCE_TABLE::iterator rrtit = m_regResourceTable.find(resID);
	RESOURCE_MOD_DESCRIPTOR* rmd = &(rrtit->second);

	if (!rmd->fAlloc)		// not modified?
	{		
		// allocate memory for resource
		_OutputDebugString (L"Allocating bytes for resource data: %d", rmd->cbOldSize + rmd->cbSize);
		rmd->pvAddress = VirtualAlloc(0, rmd->cbOldSize + rmd->cbSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	
		LPVOID lpvPrevAddr = rpit->second.first;	// original resource address

		switch (rmd->iModType)
		{
		case RR_COPY:	// copy the resource in the mem block only
		case RR_NEW:
			RtlCopyMemory (rmd->pvAddress, lpvPrevAddr, rmd->cbOldSize);
			break;

		case RR_APPEND: // append the new resource data at the last byte 
			RtlCopyMemory (rmd->pvAddress, lpvPrevAddr, rmd->cbOldSize+rmd->cbSize);
			RtlCopyMemory ((LPVOID)((DWORD)rmd->pvAddress + rmd->cbOldSize), rmd->pvResData, (SIZE_T)rmd->cbSize);
			break;

		case RR_INSERT: // insert resource at rmd->cbWhere byte of the resource
			RtlCopyMemory (rmd->pvAddress, lpvPrevAddr, rmd->cbWhere);
			RtlCopyMemory ((LPVOID)((DWORD)rmd->pvAddress + rmd->cbWhere),  rmd->pvResData, (SIZE_T)rmd->cbSize);
			RtlCopyMemory ((LPVOID)((DWORD)rmd->pvAddress + rmd->cbWhere + rmd->cbSize), 
				(LPVOID)((DWORD)lpvPrevAddr + rmd->cbWhere), 
				(SIZE_T)rmd->cbOldSize + rmd->cbSize - rmd->cbWhere); 

			break;
		}

		rmd->fAlloc = true;
		return rmd->pvAddress;
	}
	else
		OutputDebugString (L"CRM::AllocResource Returning already allocated address.");
		return rmd->pvAddress;
}

// Return a proper resource size for SizeofResource call
DWORD CResourceManager::GetFixedResourceSize (const HRSRC hr)
{
	RESOURCE_HANDLE_TABLE::const_iterator rhit = m_handleTable.find(hr);
	ULONGLONG rid = rhit->second;
	REGISTERED_RESOURCE_TABLE::iterator rrtit = m_regResourceTable.find(rid);
	
	if (rrtit->second.cbNewSize == NULL)
		rrtit->second.cbNewSize = rrtit->second.cbOldSize + rrtit->second.cbSize;
	
	return rrtit->second.cbNewSize;
}

// Save resource size for a resource ID
void CResourceManager::SetResourceSize(const ULONGLONG& ullResID, const DWORD dwSize)
{
	REGISTERED_RESOURCE_TABLE::iterator it = m_regResourceTable.find (ullResID);
	it->second.cbOldSize = dwSize;	

	WCHAR buf[MAXSTRL];
	wsprintf(buf, L"SetResource old size for res %I64d to %d bytes.", ullResID, dwSize);
	OutputDebugString(buf);
}

// Get the handle for a resource present on the injected DLL
HRSRC CResourceManager::GetDLLResourceHandle(const ULONGLONG& ulid)
{	
	REGISTERED_RESOURCE_TABLE::iterator it = m_regResourceTable.find (ulid);
	_ASSERTE(it->second.iModType == RR_NEW);
	return it->second.ri.hrsrc;
}

// Get the data handle (returned by LoadResource) for a resource present on the injected DLL
HGLOBAL CResourceManager::GetDLLResourceDataHandle(const ULONGLONG& ulid)
{
	REGISTERED_RESOURCE_TABLE::iterator it = m_regResourceTable.find (ulid);
	_ASSERTE(it->second.iModType == RR_NEW);
	return it->second.ri.hResData;
}


// Get size for a resource present on resource DLL
DWORD CResourceManager::GetDLLResourceSize(const ULONGLONG& ulid)
{
	REGISTERED_RESOURCE_TABLE::iterator it = m_regResourceTable.find (ulid);
	_ASSERTE(it->second.iModType == RR_NEW);
	return it->second.ri.dwSize;
}

// Checks if the resource is present on the registered resource table)
//
bool CResourceManager::IsResourceRegistered(const DWORD dwName, const DWORD dwType)
{
	REGISTERED_RESOURCE_TABLE::const_iterator it = 
		m_regResourceTable.find (MAKEULONGLONG(dwName,dwType));

	return (it != m_regResourceTable.end());		
}

// Check if the handle is present on the handle table
//
bool CResourceManager::IsHandleRegistered(const HRSRC hResource)
{
	RESOURCE_HANDLE_TABLE::const_iterator rhti = 
		m_handleTable.find (hResource);

	return (rhti != m_handleTable.end());		
}

// Return true for a new resource (previously non-existent)

bool CResourceManager::IsNewResource (const ULONGLONG& rid)
{
	REGISTERED_RESOURCE_TABLE::const_iterator it = 
		m_regResourceTable.find (rid);

	return (it->second.iModType == RR_NEW);
	
}

// Check if the data handle is present on the data pointer table
//
bool CResourceManager::IsDataPointerRegistered(const HGLOBAL hDataPtr)
{
	RESOURCE_POINTER_TABLE::const_iterator itrpt = m_ptrTable.find(hDataPtr);
	return (itrpt != m_ptrTable.end());		
}

// Add a handle to the handle-table with the resource ID that refers to
//
void CResourceManager::AddHandleTableEntry(const ULONGLONG& ResourceEntry, const HRSRC hResource)
{
	m_handleTable.insert(RESOURCE_HANDLE_ENTRY(hResource, ResourceEntry));	
}

// Add a resource data handle returned by LoadResource and the Res ID 
//
void CResourceManager::AddPointerTableEntry (const ULONGLONG& resID, const HGLOBAL hResData)
{
	m_ptrTable.insert (RESOURCE_POINTER_ENTRY(hResData, RESOURCE_POINTER(NULL, resID)));	
}

// Add a pointer for resource data in the pointer table
//
void CResourceManager::SetResourcePointer (const HGLOBAL hResData, LPVOID ptr)
{
	RESOURCE_POINTER_TABLE::iterator it = m_ptrTable.find (hResData);
	(it->second).first = ptr;	
}

// Finds the resource ID that corresponds to an entry in the handle table
//
ULONGLONG CResourceManager::ResourceIDFromHandle (const HRSRC hr)
{
	RESOURCE_HANDLE_TABLE::iterator it = m_handleTable.find(hr);
	return it->second;
}

// Returns resource size by HRSRC
//
DWORD CResourceManager::GetOriginalResourceSize (const HRSRC hr)
{
	ULONGLONG rid = ResourceIDFromHandle(hr);
	REGISTERED_RESOURCE_TABLE::const_iterator it = m_regResourceTable.find(rid);
	_OutputDebugString (L"GetOriginalResourceSize=%d",it->second.cbOldSize);
	return it->second.cbOldSize;
}

// Returns resource size by Resource ID
//
DWORD CResourceManager::GetOriginalResourceSize (const ULONGLONG& rid)
{
	REGISTERED_RESOURCE_TABLE::const_iterator it = m_regResourceTable.find(rid);
	_OutputDebugString (L"GetOriginalResourceSize=%d",it->second.cbOldSize);
	return it->second.cbOldSize;
}

// Finds the resource ID that corresponds to an entry in the data pointer table
//
ULONGLONG CResourceManager::ResourceIDFromDataHandle (const HGLOBAL hg)
{
	RESOURCE_POINTER_TABLE::iterator it = m_ptrTable.find(hg);
	return (it->second).second;	
}

//
//
// Dumps table info to debugging output
//
void CResourceManager::DumpTables()
{
	WCHAR buf[MAXSTRL];

	OutputDebugString(L"REGISTERED_RESOURCE_TABLE\n");
	OutputDebugString(L"TYPE+NAME	    MOD	   ADDR    WHERE   OLDSIZE    DSIZE \n");
	OutputDebugString(L"--------------------------------------------------------\n");

	for (REGISTERED_RESOURCE_TABLE::const_iterator it = m_regResourceTable.begin();
		it != m_regResourceTable.end();	it ++)
	{
		wsprintf(buf, L"%#I64x %#x %#x %#x %#x %#x\n", it->first, it->second.iModType, it->second.pvAddress, 
			it->second.cbWhere, it->second.cbOldSize, it->second.cbSize);
		OutputDebugString(buf);		
	}

	OutputDebugString(L"\nRESOURCE_HANDLE_TABLE\n");
	OutputDebugString(L"HANDLE		RESOURCE_ID *\n");
	OutputDebugString(L"----------------------------------------\n");

	for (RESOURCE_HANDLE_TABLE::const_iterator it = m_handleTable.begin();
		it != m_handleTable.end();	it ++)
	{
		wsprintf(buf, L"%#x %#I64x\n", it->first, it->second);
		OutputDebugString(buf);		
	}

	OutputDebugString(L"\nRESOURCE_POINTER_TABLE\n");
	OutputDebugString(L"DATA_HANDLE		   RESOURCE ADDRESS    R_ID\n");
	OutputDebugString(L"-------------------------------------------\n");

	for (RESOURCE_POINTER_TABLE::const_iterator it = m_ptrTable.begin();
		it != m_ptrTable.end();	it ++)
	{
		wsprintf(buf, L"%#x %#x %#I64x\n", it->first, it->second.first, it->second.second);
		OutputDebugString(buf);		
	}
	
}
