#include "Memory.h"

///
/// Helpers:
///
#define P_ACCESS_WRITE	(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE)
#define P_ACCESS_READ	(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_VM_OPERATION)
#define ARRAY_LEN(_array) (sizeof(_array)/sizeof(_array[0]))
template <class T>
SIZE_T GetStringLength(const NktMemory& mem, SIZE_T offset);

///
/// Constructors / Destructor
///
NktMemory::NktMemory()
{
	_ptr = NULL;
	_pid = 0;
}
NktMemory::NktMemory(const NktMemory& r)
{
	_ptr = r._ptr;
	_pid = r._pid;
}
NktMemory::NktMemory(DV_PTR ptr, DWORD pid)
{
	_ptr = (void*)ptr;
	_pid = pid;
}
NktMemory::NktMemory(void* ptr, DWORD pid)
{
	_ptr = ptr;
	_pid = pid;
}

///
/// CMemory::Read
///
SIZE_T NktMemory::Read(SIZE_T offset, SIZE_T size, OUT DV_PTR buffer) const
{
	DWORD oldProt = 0;
	DWORD dummie = -1;
	char* addr = (char*)_ptr + offset;
	SIZE_T bytesRead = 0;
	HRESULT hr = S_OK;
	wchar_t* errMsg = NULL;
	MEMORY_BASIC_INFORMATION memInfo;
	HANDLE hProc = OpenProcess(P_ACCESS_READ, FALSE, _pid);
	ZeroMemory(&memInfo, sizeof(memInfo));

	//Check handle:
	if (hProc == NULL)
	{
		errMsg = L"Memory Read: failed to obtain process handle.";
		hr = E_ACCESSDENIED;
	}
	else
	{
		//Check & obtain permissions:
		VirtualQueryEx(hProc, addr, &memInfo, size);
		if ((memInfo.Protect < PAGE_READONLY) && !VirtualProtectEx(hProc, addr, size, PAGE_EXECUTE_READWRITE, &oldProt))
		{
			OutputDebugStringW(L"Memory: Read: not enough permissions. Trying anyway...");
			hr = S_FALSE;
		}

		//Read memory:
		if (ReadProcessMemory(hProc, addr, (LPVOID)buffer, size, &bytesRead) == 0)
		{
			errMsg = L"Memory: Read: failed to read page.";
			hr = E_FAIL;
		}

		//If necessary, restore permissions.
		if (oldProt != 0 && !VirtualProtectEx(hProc, addr, size, oldProt, &dummie))
		{
			if(hr == S_OK)
			{
				OutputDebugStringW(L"Memory: Read: failed to restore memory previous protection.");
				hr = S_FALSE;
			}
		}

		CloseHandle(hProc);
	}

	if (FAILED(hr))
	{
		OutputDebugStringW(errMsg);
		bytesRead = 0;
	}

	return bytesRead;
}
SIZE_T NktMemory::Read(SIZE_T offset, SIZE_T size, OUT void* buffer) const
{ 
	return Read(offset, size, (DV_PTR) buffer); 
}

///
/// CMemory::Write
///
SIZE_T NktMemory::Write(SIZE_T offset, SIZE_T size, const DV_PTR src)
{
	DWORD oldProt = 0;
	DWORD dummie = -1;
	char* addr = (char*)_ptr + offset;
	HANDLE hProc = OpenProcess(P_ACCESS_WRITE, FALSE, _pid);
	HRESULT hr = S_OK;
	wchar_t* errMsg = NULL;
	SIZE_T bytesWritten = 0;

	//Check handle:
	if (hProc == NULL)
	{
		errMsg = L"Memory Read: failed to obtain process handle.";
		hr = E_ACCESSDENIED;
	}
	else
	{
		//Set write protection:
		if(VirtualProtectEx(hProc, addr, size, PAGE_READWRITE, &oldProt) == FALSE)
		{
			errMsg = (L"Memory: Write: failed to set page protection.");
			hr = E_ACCESSDENIED;
		}
		//Write memory:
		else if (WriteProcessMemory(hProc, addr, (LPCVOID)src, size, &bytesWritten) == FALSE)
		{
			errMsg = (L"Memory: Write: failed to write page.");
			hr = E_FAIL;
		}
		//Restore protection:
		else if (VirtualProtectEx(hProc, addr, size, oldProt, &dummie) == FALSE)
		{
			OutputDebugStringW(L"Memory: Write: failed to restore previous page protection.");
			hr = S_FALSE;
		}
		CloseHandle(hProc);
	}

	if (FAILED(hr))
	{
		OutputDebugStringW(errMsg);
		bytesWritten = 0;
	}

	return bytesWritten;
}
SIZE_T NktMemory::Write(SIZE_T offset, SIZE_T size, IN const void* src)
{ 
	return Write(offset, size, (DV_PTR) src);
}

///
/// CMemory::GetStringLength
///
SIZE_T NktMemory::GetStringLengthA(SIZE_T offset) const
{
	return GetStringLength<CHAR>(*this, offset);
}
SIZE_T NktMemory::GetStringLengthW(SIZE_T offset) const
{
	return GetStringLength<WCHAR>(*this, offset);
}

///
/// CMemory::GetAddress
///
DV_PTR NktMemory::GetAddress() const
{
	return (DV_PTR)_ptr;
}

///
/// CMemory::SetAddress
///
void NktMemory::SetAddress(DV_PTR ptr)
{
	_ptr = (void *)ptr;
}

///
/// CMemory::SetProcessId
///
void NktMemory::SetProcessId(DWORD pid)
{ 
	_pid = pid;
}

///
/// CMemory::GetPID
///
DWORD NktMemory::GetProcessId() const
{
	return _pid;
}

///
/// CMemory::GetMemoryInfo
///
void NktMemory::GetMemoryInfo(SIZE_T offset, OUT MEMORY_BASIC_INFORMATION* pMemInfo) const
{
	SIZE_T memInfoSize = sizeof(*pMemInfo);
	ZeroMemory(pMemInfo, memInfoSize);
	DV_PTR addr = (DV_PTR)_ptr + offset;

	// Get process handler
	HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, _pid);

	// Check hProc:
	if(hProc == NULL)
	{
		OutputDebugStringW(L"Memory::GetMemoryInfo() : Failed to obtain process handle");
		pMemInfo = NULL;
		return;
	}

	VirtualQueryEx(hProc, (void*)addr, pMemInfo, memInfoSize);

	CloseHandle(hProc);
}


///
/// CMemory::MapProtectionFlag
///
DWORD NktMemory::MapProtectionFlag( DWORD win32Protection ) const
{
	DWORD nktProtection = 0;

	if( win32Protection & PAGE_EXECUTE )
	{
		nktProtection = NktMemoryProtectionExecute;
	}
	else if( win32Protection & PAGE_EXECUTE_READ )
	{
		nktProtection = NktMemoryProtectionExecute | NktMemoryProtectionRead;
	}
	else if( win32Protection & PAGE_EXECUTE_READWRITE )
	{
		nktProtection = NktMemoryProtectionExecute | NktMemoryProtectionRead | NktMemoryProtectionWrite;
	}
	else if( win32Protection & PAGE_EXECUTE_WRITECOPY )
	{
		nktProtection = NktMemoryProtectionExecute | NktMemoryProtectionRead | NktMemoryProtectionWrite | NktMemoryProtectionWriteCopy;
	}
	else if( win32Protection & PAGE_NOACCESS )
	{	
		nktProtection = NktMemoryProtectionNoAccess;
	}
	else if( win32Protection & PAGE_READONLY )
	{
		nktProtection = NktMemoryProtectionRead;
	}
	else if( win32Protection & PAGE_READWRITE )
	{
		nktProtection = NktMemoryProtectionRead | NktMemoryProtectionWrite;
	}
	else if( win32Protection & PAGE_WRITECOPY )
	{
		nktProtection = NktMemoryProtectionRead | NktMemoryProtectionWrite | NktMemoryProtectionWriteCopy;
	}

	// these are modifiers

	if( win32Protection & PAGE_GUARD )
	{
		nktProtection = nktProtection | NktMemoryProtectionGuard;
	}

	if( win32Protection & PAGE_NOCACHE )
	{
		nktProtection = nktProtection | NktMemoryProtectionNoCache;
	}

	if( win32Protection & PAGE_WRITECOMBINE )
	{
		nktProtection = nktProtection | NktMemoryProtectionWriteCombine;
	}

	return nktProtection;
}

///
/// CMemory::GetProtection
///
DWORD NktMemory::GetProtection(SIZE_T offset, BOOL allocated /*= TRUE*/) const
{
	MEMORY_BASIC_INFORMATION memInfo;
	GetMemoryInfo(offset, &memInfo);

	DWORD protection;

	if( allocated )
	{
		protection = MapProtectionFlag( memInfo.AllocationProtect );
	}
	else
	{
		protection = MapProtectionFlag( memInfo.Protect );
	}
    
	return protection;
}

///
/// CMemory::IsExecutable
///
BOOL NktMemory::IsExecutable(SIZE_T offset, BOOL allocated /*= TRUE*/) const
{
	BOOL exec;
	try
	{
		exec = GetProtection( offset, allocated ) & NktMemoryProtectionExecute;
	}
	catch ( ... )
	{
		exec = FALSE;
	}
	return exec;
}

/************************************************************************/
/* Helpers                                                              */
/************************************************************************/
template <class T>
SIZE_T GetStringLength(const NktMemory& mem, SIZE_T offset)
{
	T ch = NULL;
	T buffer[0x80];
	bool parsing = true;
	SIZE_T len = 0;

	while (parsing)
	{
		mem.Read(offset, sizeof(buffer), buffer);
		for (int i = 0; i < ARRAY_LEN(buffer); i++, len++)
		{
			if (buffer[i] == 0)
			{
				parsing = false;
				break;
			}
		}
		offset += sizeof(buffer);
	}

	return len;
}