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

#ifndef RESOURCE_MANAGER_H
#define	RESOURCE_MANAGER_H
#include <windows.h>
#include <map>

// resource-modification constants

#define	RR_APPEND		0x1
#define RR_INSERT		0x2
#define RR_REPLACE		0x3
#define RR_COPY			0x4
#define RR_NEW			0xFF
#define RR_NONE			RR_COPY

// Data for resources residing on injected-DLL resource section
typedef struct tagRESOURCEINFO
{
	UINT	uResId;
	LPWSTR	wszResType;
	HRSRC	hrsrc;
	DWORD	dwSize;	
	HGLOBAL	hResData;
	LPVOID	pvData;

} RESOURCEINFO;

class CResourceManager
{
	typedef struct __RMD
	{
		UINT	iModType;
		LPVOID	pvAddress;
		LPVOID	pvResData;
		DWORD	cbOldSize;		// original size of resource
		DWORD	cbSize;			// size of additional resource data
		DWORD	cbNewSize;		// modified resource size
		UINT	cbWhere;		// position to modify resource 
								// valid for RR_INSERT only
		bool	fAlloc;			// modified resource allocated?
		RESOURCEINFO ri;		// data of injected-DLL resident resource		

	} RESOURCE_MOD_DESCRIPTOR;

	typedef std::pair<LPVOID, ULONGLONG> RESOURCE_POINTER;
	typedef std::pair<HGLOBAL, RESOURCE_POINTER>			RESOURCE_POINTER_ENTRY;
	typedef std::pair<ULONGLONG, RESOURCE_MOD_DESCRIPTOR>	RESOURCE_ENTRY;
	typedef std::pair<HRSRC, ULONGLONG>						RESOURCE_HANDLE_ENTRY;
	
	typedef std::map<ULONGLONG, RESOURCE_MOD_DESCRIPTOR>	REGISTERED_RESOURCE_TABLE;
	typedef std::map<HRSRC, ULONGLONG>						RESOURCE_HANDLE_TABLE;
	typedef std::map<HGLOBAL, RESOURCE_POINTER>				RESOURCE_POINTER_TABLE;

	REGISTERED_RESOURCE_TABLE	m_regResourceTable;
	RESOURCE_HANDLE_TABLE		m_handleTable;
	RESOURCE_POINTER_TABLE		m_ptrTable;
	
public:
	
	CResourceManager(void);
	~CResourceManager(void);

	void RegisterResource (DWORD type, DWORD name, LPVOID resData, DWORD cbResDataSize,
						   UINT resModType, UINT cbWhere = 0);	
	void RegisterNewResource (DWORD dwType, DWORD dwName, const RESOURCEINFO&);
	bool IsResourceRegistered (const DWORD, const DWORD);
	bool IsHandleRegistered (const HRSRC);
	bool IsDataPointerRegistered (const HGLOBAL);
	bool IsNewResource (const ULONGLONG&);
	void AddHandleTableEntry (const ULONGLONG& ResourceEntry, const HRSRC);
	void AddPointerTableEntry (const ULONGLONG&, const HGLOBAL);
	void SetResourcePointer (const HGLOBAL, LPVOID);
	ULONGLONG ResourceIDFromHandle (const HRSRC);
	ULONGLONG ResourceIDFromDataHandle (const HGLOBAL);
	LPVOID AllocResource (const HGLOBAL);
	void SetResourceSize(const ULONGLONG&, const DWORD);
	DWORD GetOriginalResourceSize (const HRSRC);
	DWORD GetOriginalResourceSize (const ULONGLONG&);
	DWORD GetFixedResourceSize (const HRSRC);
	HRSRC GetDLLResourceHandle(const ULONGLONG&);
	DWORD GetDLLResourceSize(const ULONGLONG&);
	HGLOBAL GetDLLResourceDataHandle(const ULONGLONG& ulid);

	void DumpTables ();
	
};

#endif // RESOURCE_MANAGER_H
