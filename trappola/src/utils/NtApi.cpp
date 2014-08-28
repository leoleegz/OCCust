#include "ntapi.h"
#include "NtUnDoc.h"
#include <tchar.h>
#include <stdio.h>
#include <stdarg.h>
#pragma warning(disable:4005)
#include <ntstatus.h>
#include <crtdbg.h>
#include <malloc.h>

///
/// Types:
///
typedef ULONG_PTR KPRIORITY;
typedef struct _THREAD_BASIC_INFORMATION {
	NTSTATUS                ExitStatus;
	PVOID                   TebBaseAddress;
	ClientId               ClientId;
	KAFFINITY               AffinityMask;
	KPRIORITY               Priority;
	KPRIORITY               BasePriority;
} THREAD_BASIC_INFORMATION, *PTHREAD_BASIC_INFORMATION;

///
/// Helpers:
///
void SetUnicodeString(UNICODE_STRING& ustr, LPCWSTR txt);
#define InitializeObjectAttributes( p, n, a, r, s ) { \
	(p)->Length = sizeof( OBJECT_ATTRIBUTES );          \
	(p)->RootDirectory = r;                             \
	(p)->Attributes = a;                                \
	(p)->ObjectName = n;                                \
	(p)->SecurityDescriptor = s;                        \
	(p)->SecurityQualityOfService = NULL;               \
}
#define ANSI_TO_WIDE(_ansi, _wide) \
	size_t l = strlen(_ansi); \
	size_t wl = MultiByteToWideChar(CP_ACP, 0, _ansi, (int)l, NULL,0); \
	_wide = static_cast<LPWSTR>(alloca(++wl*sizeof(WCHAR))); \
	if (::MultiByteToWideChar(CP_ACP, 0, _ansi, (int)l, _wide, (int)wl) == 0) \
	_wide = NULL;



/**
NtApiInitializer
*/
class NktNtApiInitializer
{
public:
	NktNtApiInitializer()
	{
		NktNtApi::Initialize();
	}
} NtApiInitializerInst;

///
/// NtApi: Statics
///
bool NktNtApi::initialized = false;
NktNtApi::LdrGetProcedureAddress NktNtApi::getProcAddress = NULL;
NktNtApi::NtSuspendThread NktNtApi::suspendThread = NULL;
NktNtApi::DbgPrint NktNtApi::dbgPrint = NULL;
NktNtApi::RtlInitAnsiString NktNtApi::initAnsiString = NULL;
NktNtApi::RtlInitUnicodeString NktNtApi::initUnicodeString = NULL;
NktNtApi::NtCurrentTeb NktNtApi::ntCurrentTeb = NULL;
NktNtApi::NtSetEvent NktNtApi::ntSetEvent = NULL;
NktNtApi::NtPulseEvent NktNtApi::ntPulseEvent = NULL;
NktNtApi::NtResetEvent NktNtApi::ntResetEvent = NULL;
NktNtApi::NtClose NktNtApi::ntClose = NULL;
NktNtApi::NtCreateEvent NktNtApi::ntCreateEvent = NULL;
NktNtApi::NtOpenEvent NktNtApi::ntOpenEvent = NULL;
NktNtApi::NtQueryInformationThread NktNtApi::ntQueryInformationThread = NULL;
NktNtApi::NtLdrGetDllHandle NktNtApi::ntLdrGetDllHandle = NULL;
SYSTEM_INFO NktNtApi::systemInfo = { 0 };

///
/// Helpers:
///
void GetClientID(ClientId& cid)
{
	PUndocTeb teb = NULL;
	// calculate the offset of the clientId (it should work in 64b
	SIZE_T offset = (INT_PTR) &teb->Cid - (INT_PTR) teb;
	INT_PTR cteb = (SIZE_T)NktNtApi::CurrentTEB();

	memcpy(&cid, (const void*)(cteb + offset), sizeof(ClientId));
}

///
/// Constructors / Destructor
///
void NktNtApi::Initialize()
{
	if (initialized)
		return;

	HMODULE hMod = ::GetModuleHandleW(L"ntdll.dll"); _ASSERT(hMod);
	initAnsiString = (RtlInitAnsiString) ::GetProcAddress(hMod, "RtlInitAnsiString");
	initUnicodeString = (RtlInitUnicodeString) ::GetProcAddress(hMod, "RtlInitUnicodeString");
	getProcAddress = (LdrGetProcedureAddress)::GetProcAddress(hMod, "LdrGetProcedureAddress");
	suspendThread = (NtSuspendThread)::GetProcAddress(hMod, "NtSuspendThread");
	dbgPrint = (DbgPrint)::GetProcAddress(hMod, "DbgPrint");
	ntCurrentTeb = (NtCurrentTeb)::GetProcAddress(hMod, "NtCurrentTeb");
	ntSetEvent = (NtSetEvent)::GetProcAddress(hMod, "NtSetEvent");
	ntResetEvent = (NtResetEvent)::GetProcAddress(hMod, "NtPulseEvent");
	ntPulseEvent = (NtPulseEvent)::GetProcAddress(hMod, "NtResetEvent");
	ntClose = (NtClose)::GetProcAddress(hMod, "NtClose");
	ntCreateEvent = (NtCreateEvent) ::GetProcAddress(hMod, "NtCreateEvent");
	ntOpenEvent = (NtOpenEvent) ::GetProcAddress(hMod, "NtOpenEvent");
	ntQueryInformationThread = (NtQueryInformationThread) ::GetProcAddress(hMod, "NtQueryInformationThread");
	ntLdrGetDllHandle = (NtLdrGetDllHandle) ::GetProcAddress(hMod, "LdrGetDllHandle");

	ZeroMemory(&systemInfo, sizeof(SYSTEM_INFO));
	::GetSystemInfo(&systemInfo);

	initialized = true;
}

///
/// NktNtApi::CloseHandle
///
BOOL NktNtApi::CloseHandle(HANDLE h)
{
	NTSTATUS st = ntClose(h);
	return st == 0;
}

///
/// NktNtApi::SuspendThread
///
void NktNtApi::SuspendThread(HANDLE hthread)
{
	_ASSERT(initialized);
	suspendThread(hthread, NULL);
}

///
/// NktNtApi::GetCurrentThreadId
///
DWORD NktNtApi::GetCurrentThreadId()
{
	_ASSERT(initialized);
	ClientId cid;
	GetClientID(cid);
	return cid.UniqueThreadId;
}

///
/// NktNtApi::GetCurrentProcessId
///
DWORD NktNtApi::GetCurrentProcessId()
{
	_ASSERT(initialized);
	ClientId cid;
	GetClientID(cid);
	return cid.UniqueProcessId;
}

///
/// NktNtApi::GetProcAddress
///
FARPROC NktNtApi::GetProcAddress(IN HMODULE hModule, IN LPCSTR lpProcName)
{
	_ASSERT(initialized);
	FARPROC ret = NULL;
	char buffer[256];
	STRING ntString;
	ntString.Buffer = buffer;
	initAnsiString(&ntString, lpProcName);
	getProcAddress(hModule, &ntString, 0, (PVOID*)&ret);
	return ret;
}

///
/// NktNtApi::CurrentTEB
///
PTEB NktNtApi::CurrentTEB()
{
	_ASSERT(initialized);
	return ntCurrentTeb();
}

///
/// NktNtApi::GetThreadTEB
///
PTEB NktNtApi::GetThreadTEB(DWORD tid)
{
	THREAD_BASIC_INFORMATION tbInfo;
	PTEB teb = NULL;

	HANDLE hThread = OpenThread(THREAD_QUERY_INFORMATION, FALSE, tid);
	if (!hThread) return teb;

	NTSTATUS ret = ntQueryInformationThread(hThread, (THREADINFOCLASS)0, &tbInfo, sizeof(tbInfo), NULL);
	if (ret == STATUS_SUCCESS)
		teb = (PTEB)tbInfo.TebBaseAddress;

	CloseHandle(hThread);
	return teb;
}

///
/// NktNtApi::GetPageSize
///
DWORD NktNtApi::GetPageSize()
{
	_ASSERT(initialized);
	return systemInfo.dwPageSize;
}

///
/// NktNtApi::CreateEvent
///
HANDLE NktNtApi::CreateEvent(LPSECURITY_ATTRIBUTES lpEventAttributes, BOOL bManualReset, BOOL bInitialState, LPCWSTR lpName)
{
	EVENT_TYPE type = (bManualReset)? NotificationEvent : SynchronizationEvent;
	HANDLE hEvent = 0;
	OBJECT_ATTRIBUTES obj;
	UNICODE_STRING uname, *puname = NULL;
	if (lpName)
	{
		SetUnicodeString(uname, lpName);
		puname = &uname;
	}
	InitializeObjectAttributes(&obj, &uname, 0, 0, 0);
	NTSTATUS ret = ntCreateEvent(&hEvent, EVENT_ALL_ACCESS, &obj, type, bInitialState);
	return hEvent;
}
HANDLE NktNtApi::CreateEvent(LPSECURITY_ATTRIBUTES lpEventAttributes, BOOL bManualReset, BOOL bInitialState, LPCSTR lpName)
{
	LPWSTR wname = NULL;
	if (lpName)
	{
		ANSI_TO_WIDE(lpName, wname);
		if (wname == NULL)
		{
			OutputDebugStringW(L"NtApi: CreateEvent: Can't create event with provided name.");
			return INVALID_HANDLE_VALUE;
		}
	}
	return CreateEvent(lpEventAttributes, bManualReset, bInitialState, wname);
}

///
/// NktNtApi::OpenEvent
///
HANDLE NktNtApi::OpenEvent(DWORD dwDesiredAccess, BOOL bInheritHandle, LPCWSTR lpName)
{
	_ASSERT(bInheritHandle == FALSE);
	HANDLE hEvent = 0;
	OBJECT_ATTRIBUTES obj;
	UNICODE_STRING uname, *puname = NULL;
	if (lpName)
	{
		SetUnicodeString(uname, lpName);
		puname = &uname;
	}
	InitializeObjectAttributes(&obj, puname, 0, 0, 0);
	NTSTATUS ret = ntOpenEvent(&hEvent, dwDesiredAccess, &obj); //_ASSERT(ret == STATUS_SUCCESS);
	return hEvent;
}
HANDLE NktNtApi::OpenEvent(DWORD dwDesiredAccess, BOOL bInheritHandle, LPCSTR lpName)
{
	LPWSTR wname = NULL;
	if (lpName)
	{
		ANSI_TO_WIDE(lpName, wname);
	}
	return OpenEvent(dwDesiredAccess, bInheritHandle, wname);
}

///
/// NktNtApi::SetEvent
///
BOOL NktNtApi::SetEvent(IN HANDLE hEvent)
{
	_ASSERT(initialized);
	NTSTATUS ret = ntSetEvent(hEvent, 0);
	return ret == STATUS_SUCCESS;
}

///
/// NktNtApi::SetEvent
///
BOOL NktNtApi::PulseEvent(IN HANDLE hEvent)
{
	_ASSERT(initialized);
	NTSTATUS ret = ntPulseEvent(hEvent, 0);
	return ret == STATUS_SUCCESS;
}

///
/// NktNtApi::SetEvent
///
BOOL NktNtApi::ResetEvent(IN HANDLE hEvent)
{
	_ASSERT(initialized);
	NTSTATUS ret = ntResetEvent(hEvent, 0);
	return ret == STATUS_SUCCESS;
}


///
/// NktNtApi::DebugPrint
///
void __declspec(naked) NktNtApi::DebugPrint(IN LPCSTR msg, ...)
{
	_ASSERT(initialized);
	_asm {
		jmp [NktNtApi::dbgPrint]
	}
}

///
/// NktNtApi::GetModuleHandle
///
HMODULE NktNtApi::RawGetModuleHandle(IN LPCWSTR modName)
{
	HANDLE handle;
	UNICODE_STRING ustr;
	SetUnicodeString(ustr, modName);
	NTSTATUS status = ntLdrGetDllHandle(0, 0, &ustr, &handle);
	return (status == STATUS_SUCCESS)? (HMODULE)handle : NULL;
}
HMODULE NktNtApi::RawGetModuleHandle(IN LPCSTR modName)
{
	size_t l = strlen(modName);
	WCHAR* buffer = (WCHAR*)alloca((l+1)*sizeof(WCHAR));
	int conv = MultiByteToWideChar(CP_ACP, 0, modName, (int)l+1, buffer, (int)(l+1)*sizeof(WCHAR));
	return (conv != 0)? GetModuleHandle(buffer) : NULL;
}

/************************************************************************/
/* Helpers                                                              */
/************************************************************************/
void SetUnicodeString(UNICODE_STRING& ustr, LPCWSTR txt)
{
	ustr.Buffer = (PWSTR)txt;
	ustr.Length = static_cast<USHORT>(wcslen(ustr.Buffer) << 1);
	ustr.MaximumLength = ustr.Length + sizeof(WCHAR);
}