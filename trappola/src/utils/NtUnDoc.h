#ifndef _NTUNDOC_H_
#define _NTUNDOC_H_

typedef struct _ClientId
{
	DWORD UniqueProcessId;
	DWORD UniqueThreadId;
} ClientId, *PClientId;

typedef struct _UndocTeb
{
	NT_TIB Tib;                         // 00h
	PVOID EnvironmentPointer;           // 1Ch
	ClientId Cid;                      // 20h
	PVOID ActiveRpcInfo;                // 28h
	PVOID ThreadLocalStoragePointer;    // 2Ch
	PPEB Peb;                           // 30h
	ULONG LastErrorValue;               // 34h
	ULONG CountOfOwnedCriticalSections; // 38h
	PVOID CsrClientThread;              // 3Ch
	PVOID Win32ThreadInfo;              // 40h
	ULONG Win32ClientInfo[0x1F];        // 44h
	PVOID WOW32Reserved;                // C0h
	ULONG CurrentLocale;                // C4h
	ULONG FpSoftwareStatusRegister;     // C8h
	PVOID SystemReserved1[0x36];        // CCh
	PVOID Spare1;                       // 1A4h
	LONG ExceptionCode;                 // 1A8h
	ULONG SpareBytes1[0x28];            // 1ACh
	PVOID SystemReserved2[0xA];         // 1D4h
	//   GDI_TEB_BATCH GdiTebBatch;          // 1FCh
	ULONG gdiRgn;                       // 6DCh
	ULONG gdiPen;                       // 6E0h
	ULONG gdiBrush;                     // 6E4h
	ClientId RealClientId;             // 6E8h
	PVOID GdiCachedProcessHandle;       // 6F0h
	ULONG GdiClientPID;                 // 6F4h
	ULONG GdiClientTID;                 // 6F8h
	PVOID GdiThreadLocaleInfo;          // 6FCh
	PVOID UserReserved[5];              // 700h
	PVOID glDispatchTable[0x118];       // 714h
	ULONG glReserved1[0x1A];            // B74h
	PVOID glReserved2;                  // BDCh
	PVOID glSectionInfo;                // BE0h
	PVOID glSection;                    // BE4h
	PVOID glTable;                      // BE8h
	PVOID glCurrentRC;                  // BECh
	PVOID glContext;                    // BF0h
	NTSTATUS LastStatusValue;           // BF4h
	UNICODE_STRING StaticUnicodeString; // BF8h
	WCHAR StaticUnicodeBuffer[0x105];   // C00h
	PVOID DeallocationStack;            // E0Ch
	PVOID TlsSlots[0x40];               // E10h
	LIST_ENTRY TlsLinks;                // F10h
	PVOID Vdm;                          // F18h
	PVOID ReservedForNtRpc;             // F1Ch
	PVOID DbgSsReserved[0x2];           // F20h
	ULONG HardErrorDisabled;            // F28h
	PVOID Instrumentation[0x10];        // F2Ch
	PVOID WinSockData;                  // F6Ch
	ULONG GdiBatchCount;                // F70h
	ULONG Spare2;                       // F74h
	ULONG Spare3;                       // F78h
	ULONG Spare4;                       // F7Ch
	PVOID ReservedForOle;               // F80h
	ULONG WaitingOnLoaderLock;          // F84h
} UndocTeb, *PUndocTeb;

#ifndef _UNDOC_PEB_
#define _UNDOC_PEB_

typedef PVOID* PPVOID;
typedef void (*PPEBLOCKROUTINE)(PVOID PebLock); 

//
// _PEB_LDR_DATA
//
typedef struct _PEB_LDR_DATA 
{
   ULONG                   Length;
   BOOLEAN                 Initialized;
   PVOID                   SsHandle;
   LIST_ENTRY              InLoadOrderModuleList;
   LIST_ENTRY              InMemoryOrderModuleList;
   LIST_ENTRY              InInitializationOrderModuleList;
} PEB_LDR_DATA, *PPEB_LDR_DATA;

//
// _RTL_DRIVE_LETTER_CURDIR
//
typedef struct _RTL_DRIVE_LETTER_CURDIR 
{
   USHORT                  Flags;
   USHORT                  Length;
   ULONG                   TimeStamp;
   UNICODE_STRING          DosPath;
} RTL_DRIVE_LETTER_CURDIR, *PRTL_DRIVE_LETTER_CURDIR;

//
// _RTL_USER_PROCESS_PARAMETERS
//
typedef struct _RTL_USER_PROCESS_PARAMETERS 
{
   ULONG                   MaximumLength;
   ULONG                   Length;
   ULONG                   Flags;
   ULONG                   DebugFlags;
   PVOID                   ConsoleHandle;
   ULONG                   ConsoleFlags;
   HANDLE                  StdInputHandle;
   HANDLE                  StdOutputHandle;
   HANDLE                  StdErrorHandle;
   UNICODE_STRING          CurrentDirectoryPath;
   HANDLE                  CurrentDirectoryHandle;
   UNICODE_STRING          DllPath;
   UNICODE_STRING          ImagePathName;
   UNICODE_STRING          CommandLine;
   PVOID                   Environment;
   ULONG                   StartingPositionLeft;
   ULONG                   StartingPositionTop;
   ULONG                   Width;
   ULONG                   Height;
   ULONG                   CharWidth;
   ULONG                   CharHeight;
   ULONG                   ConsoleTextAttributes;
   ULONG                   WindowFlags;
   ULONG                   ShowWindowFlags;
   UNICODE_STRING          WindowTitle;
   UNICODE_STRING          DesktopName;
   UNICODE_STRING          ShellInfo;
   UNICODE_STRING          RuntimeData;
   RTL_DRIVE_LETTER_CURDIR DLCurrentDirectory[0x20];
} RTL_USER_PROCESS_PARAMETERS, *PRTL_USER_PROCESS_PARAMETERS;

//
// _PEB_FREE_BLOCK
//
typedef struct _PEB_FREE_BLOCK 
{
   _PEB_FREE_BLOCK         *Next;
   ULONG                   Size;
} PEB_FREE_BLOCK, *PPEB_FREE_BLOCK;


//
// _UNDOC_PEB
//
typedef struct _UNDOC_PEB 
{
   BOOLEAN                 InheritedAddressSpace;
   BOOLEAN                 ReadImageFileExecOptions;
   BOOLEAN                 BeingDebugged;
   BOOLEAN                 Spare;
   HANDLE                  Mutant;
   PVOID                   ImageBaseAddress;
   PPEB_LDR_DATA           LoaderData;
   PRTL_USER_PROCESS_PARAMETERS ProcessParameters;
   PVOID                   SubSystemData;
   PVOID                   ProcessHeap;
   PVOID                   FastPebLock;
   PPEBLOCKROUTINE         FastPebLockRoutine;
   PPEBLOCKROUTINE         FastPebUnlockRoutine;
   ULONG                   EnvironmentUpdateCount;
   PPVOID                  KernelCallbackTable;
   PVOID                   EventLogSection;
   PVOID                   EventLog;
   PPEB_FREE_BLOCK         FreeList;
   ULONG                   TlsExpansionCounter;
   PVOID                   TlsBitmap;
   ULONG                   TlsBitmapBits[0x2];
   PVOID                   ReadOnlySharedMemoryBase;
   PVOID                   ReadOnlySharedMemoryHeap;
   PPVOID                  ReadOnlyStaticServerData;
   PVOID                   AnsiCodePageData;
   PVOID                   OemCodePageData;
   PVOID                   UnicodeCaseTableData;
   ULONG                   NumberOfProcessors;
   ULONG                   NtGlobalFlag;
   BYTE                    Spare2[0x4];
   LARGE_INTEGER           CriticalSectionTimeout;
   ULONG                   HeapSegmentReserve;
   ULONG                   HeapSegmentCommit;
   ULONG                   HeapDeCommitTotalFreeThreshold;
   ULONG                   HeapDeCommitFreeBlockThreshold;
   ULONG                   NumberOfHeaps;
   ULONG                   MaximumNumberOfHeaps;
   PPVOID                  *ProcessHeaps;
   PVOID                   GdiSharedHandleTable;
   PVOID                   ProcessStarterHelper;
   PVOID                   GdiDCAttributeList;
   PVOID                   LoaderLock;
   ULONG                   OSMajorVersion;
   ULONG                   OSMinorVersion;
   ULONG                   OSBuildNumber;
   ULONG                   OSPlatformId;
   ULONG                   ImageSubSystem;
   ULONG                   ImageSubSystemMajorVersion;
   ULONG                   ImageSubSystemMinorVersion;
   ULONG                   GdiHandleBuffer[0x22];
   ULONG                   PostProcessInitRoutine;
   ULONG                   TlsExpansionBitmap;
   BYTE                    TlsExpansionBitmapBits[0x80];
   ULONG                   SessionId;
} UNDOC_PEB, *PUNDOC_PEB;

#endif // _UNDOC_PEB_

#endif //_NTUNDOC_H_