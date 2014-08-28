#ifndef _MEMORY_H_
#define _MEMORY_H_

#include "types.h"
#include "yasper.h"

/**
Memory Page Protections
*/
typedef enum _NktMemoryProtection
{
	NktMemoryProtectionNoAccess = 0,
	NktMemoryProtectionExecute = 1,
	NktMemoryProtectionRead = 2,
	NktMemoryProtectionWrite = 4,
	NktMemoryProtectionWriteCopy = 8,
	NktMemoryProtectionGuard = 16,
	NktMemoryProtectionNoCache = 32,
	NktMemoryProtectionWriteCombine = 64
} NktMemoryProtection;

/**
Class to provide memory access in any process.
*/
class NktMemory
{
public:
	NktMemory();
	NktMemory(const NktMemory&);
	NktMemory(DV_PTR ptr, DWORD pid = GetCurrentProcessId());
	NktMemory(void* ptr, DWORD pid = GetCurrentProcessId());

	/**
	Read a region of memory. <br>
	buffer must be big enough.
	*/
	SIZE_T Read(SIZE_T offset, SIZE_T size, OUT DV_PTR buffer) const;
	SIZE_T Read(SIZE_T offset, SIZE_T size, OUT void* buffer) const;

	/**
	Get the ASCII string length pointed at offset.
	*/
	SIZE_T GetStringLengthA(SIZE_T offset) const;
	SIZE_T GetStringLengthW(SIZE_T offset) const;

	/**
	Write into a region of memory.
	*/
	SIZE_T Write(SIZE_T offset, SIZE_T size, IN const DV_PTR src);
	SIZE_T Write(SIZE_T offset, SIZE_T size, IN const void* src);

	/**
	Retrieves the protections flags.<br>
	@allocated: if TRUE return the protection when it was allocated, instead of protection.
	*/
	DWORD GetProtection(SIZE_T offset, BOOL allocated = TRUE) const;

	/**
	Returns if the memory address is executable.<br>
	@allocated: if TRUE return the protection when it was allocated, instead of protection.
	*/
	BOOL IsExecutable(SIZE_T offset, BOOL allocated = TRUE) const;

	/**
	Get current address.
	*/
	DV_PTR GetAddress() const;

	/**
	Set the base address of the memory. If this address is not zero any operation with the object will be summed to this address.
	*/
	void SetAddress(DV_PTR ptr);

	/**
	Get process id.
	*/
	DWORD GetProcessId() const;

	/**
	Change Process id
	*/
	void SetProcessId(DWORD pid);

public:
	/**
	Change protection on memory segment.
	@returns previous protection.
	*/
	static DWORD SetProtection(HANDLE hProc, const DV_PTR addr, SIZE_T size, DWORD prot);
	static DWORD SetProtection(HANDLE hProc, const void* addr, SIZE_T size, DWORD prot);

protected:
	/**
	Retrieves a MEMORY_BASIC_INFORMATION from an offset
	*/
	void GetMemoryInfo(SIZE_T offset, OUT MEMORY_BASIC_INFORMATION* pMemInfo) const;

	/**
	Build a MemoryProtections based on a DWORD
	*/
	DWORD MapProtectionFlag( DWORD win32Protection ) const;

protected:
	DWORD _pid;
	void* _ptr;
};

#endif //_MEMORY_H_