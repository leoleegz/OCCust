#ifndef _NTAPI_H_
#define _NTAPI_H_

/**
Define use of WinNT from W2K
*/
#ifndef _WIN32_WINNT
	#define _WIN32_WINNT 0x0500
#else
	#if (_WIN32_WINNT < 0x0500)
		#pragma message(__FILE__ "Warning: WIN32_WINNT flag redifined to 0x0500.")
		#undef _WIN32_WINNT
		#define _WIN32_WINNT 0x0500
	#endif // _WIN32_WINNT
#endif

#include <WTypes.h>
#include <winternl.h>

/**
Raw call to functions in ntdll
PANSI_STRING needs WINVER >= 0x0500 //Windows 2000
*/
class NktNtApi
{
friend class NktNtApiInitializer;
public:
	typedef enum _EVENT_TYPE
	{
		NotificationEvent,
		SynchronizationEvent
	} EVENT_TYPE, *PEVENT_TYPE;

	// NTDLL API Declarations:
	typedef void (NTAPI *NtSuspendThread)(IN HANDLE ThreadHandle, OUT PULONG PreviousSuspendCount OPTIONAL );
	typedef void (__cdecl *DbgPrint)(IN LPCSTR Format, ... );
	typedef void (NTAPI *LdrGetProcedureAddress)(IN HMODULE ModuleHandle, IN PANSI_STRING FunctionName OPTIONAL, IN WORD Oridinal OPTIONAL, OUT PVOID *FunctionAddress );
	typedef void (NTAPI *RtlInitAnsiString)(PANSI_STRING DestinationString, PCSZ SourceString);
	typedef void (NTAPI *RtlInitUnicodeString)(PUNICODE_STRING DestinationString, PCWSTR SourceString);
	typedef PTEB (NTAPI *NtCurrentTeb)(void);
	typedef NTSTATUS (NTAPI *NtSetEvent)(IN HANDLE, OUT PLONG prevState);
	typedef NTSTATUS (NTAPI *NtResetEvent)(IN HANDLE, OUT PLONG prevState);
	typedef NTSTATUS (NTAPI *NtPulseEvent)(IN HANDLE, OUT PLONG prevState);
	typedef NTSTATUS (NTAPI *NtClose)(IN HANDLE Handle);
	typedef NTSTATUS (NTAPI *NtCreateEvent)(OUT PHANDLE EventHandle,
											IN ACCESS_MASK DesiredAccess,
											IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
											IN EVENT_TYPE EventType,
											IN BOOLEAN InitialState);
	typedef NTSTATUS (NTAPI *NtOpenEvent)(OUT PHANDLE EventHandle,
										  IN ACCESS_MASK DesiredAccess,
										  IN POBJECT_ATTRIBUTES ObjectAttributes);
	typedef NTSTATUS (NTAPI *NtQueryInformationThread)(IN HANDLE ThreadHandle,
													   IN THREADINFOCLASS ThreadInformationClass,
													   OUT PVOID ThreadInformation,
													   IN ULONG ThreadInformationLength,
													   OUT PULONG ReturnLength OPTIONAL);
	typedef NTSTATUS (NTAPI *NtLdrGetDllHandle) (IN PWORD pwPath OPTIONAL,
												 IN PVOID Unused OPTIONAL,
												 IN PUNICODE_STRING ModuleFileName,
												 OUT PHANDLE pHModule);


public:
	/**
	Suspend requested thread.
	*/
	static void SuspendThread(HANDLE hthread);

	/**
	Print debug message.
	*/
	static void DebugPrint(IN LPCSTR msg, ...);

	/**
	Get Procedure address.
	*/
	static FARPROC GetProcAddress(IN HMODULE hModule, IN LPCSTR lpProcName);

	/**
	Get inproc module handle.
	*/
	static HMODULE RawGetModuleHandle(IN LPCWSTR lpModuleName);
	static HMODULE RawGetModuleHandle(IN LPCSTR lpModuleName);

	/**
	Get TEB Structure for calling thread.
	*/
	static PTEB CurrentTEB();

	/**
	Gets pointer to the TEB structure of any thread.
	*/
	static PTEB GetThreadTEB(DWORD tid);

	/**
	Get ID for calling thread.
	*/
	static DWORD GetCurrentThreadId();

	/**
	Get ID for calling process.
	*/
	static DWORD GetCurrentProcessId();

	/**
	Get Paging size.
	*/
	static DWORD GetPageSize();

	/**
	Win32 event's functions.
	*/
	//Name MUST be in the form of "\BaseNamedObjects\<name>"
	static HANDLE CreateEvent(LPSECURITY_ATTRIBUTES lpEventAttributes,
							  BOOL bManualReset,
							  BOOL bInitialState,
							  LPCSTR lpName);
	static HANDLE CreateEvent(LPSECURITY_ATTRIBUTES lpEventAttributes,
							  BOOL bManualReset,
							  BOOL bInitialState,
							  LPCWSTR lpName);
	static HANDLE OpenEvent(DWORD dwDesiredAccess, 
							BOOL bInheritHandle,
							LPCSTR lpName);
	static HANDLE OpenEvent(DWORD dwDesiredAccess, 
							BOOL bInheritHandle,
							LPCWSTR lpName);
	static BOOL SetEvent(IN HANDLE);
	static BOOL ResetEvent(IN HANDLE);
	static BOOL PulseEvent(IN HANDLE);

	/**
	CloseHandle.
	WARNING: This will call NtClose, which is not suitable for *ANY* handle.
	*/
	static BOOL CloseHandle(HANDLE h);

protected:
	/**
	Internal initializer.
	*/
	static void Initialize();

protected:
	static DbgPrint dbgPrint;
	static NtSuspendThread suspendThread;
	static LdrGetProcedureAddress getProcAddress;
	static RtlInitAnsiString initAnsiString;
	static RtlInitUnicodeString initUnicodeString;
	static NtCurrentTeb ntCurrentTeb;
	static NtSetEvent ntSetEvent;
	static NtResetEvent ntResetEvent;
	static NtPulseEvent ntPulseEvent;
	static NtClose ntClose;
	static NtCreateEvent ntCreateEvent;
	static NtOpenEvent ntOpenEvent;
	static NtQueryInformationThread ntQueryInformationThread;
	static NtLdrGetDllHandle ntLdrGetDllHandle;

	static SYSTEM_INFO systemInfo;
	static bool initialized;
};

#endif // _NTAPI_H_