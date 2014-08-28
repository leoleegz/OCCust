#ifndef _DV_TYPES_H_ 
#define _DV_TYPES_H_

/**
This file contains the main and basic structures.
*/

//Includes:
#include <WTypes.h>
#include <string>
#include <vector>
#include "List.h"
#include "enums.h"

#ifdef _DEBUG
	# define NODEFAULT   _ASSERT(0)
#else
	# define NODEFAULT   __assume(0)
#endif

#define ABSTRACT 0
#define INVALID_INDEX (-1)

#define QUERY_TOKEN TEXT("!")

#define DV_PTR __int3264
#define DV_HOOK_ID long long

#define REGISTER_SIZE (sizeof(void*))

#define STACK_SIZE 256*1024
#define STACK_MAX_SIZE 1024*1024

//Asm code types:
typedef unsigned char asmcode;
typedef unsigned short asmcodew;

#define JMP_SIZE (sizeof(asmcode) + sizeof(DV_PTR))

typedef NktList<DWORD> NktIdList;
typedef NktList<GUID> NktGuidList;
typedef std::basic_string<TCHAR> NktString;
typedef std::vector<NktString> NktStringVector;
	
/************************************************************************/
/* Registers:                                                           */
/* LastError, EFLAGS, EAX, ECX, EDX, EBX, original ESP, EBP, ESI, EDI	*/
/************************************************************************/
typedef struct _NktRegisters
{
	INT_PTR lastError, EFLAGS, EDI, ESI, EBP, ESP, EBX, EDX, ECX, EAX;
} NktRegisters;

/**
Function Call Context
*/
typedef struct
{
	LPCVOID pms;
	NktRegisters* regs;
	INT_PTR tag;
} NktCallContext;

#endif //_DV_TYPES_H_