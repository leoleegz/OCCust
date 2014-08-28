/*
*
*  This file is
*    Copyright (C) 2006-2008 Nektra S.A.
*  
*  This program is free software; you can redistribute it and/or modify
*  it under the terms of the GNU Lesser General Public License as published by
*  the Free Software Foundation; either version 2, or (at your option)
*  any later version.
*  
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*  
*/
#include "utils.h"
#include <Windows.h>
#include <Shlwapi.h>
#include <TlHelp32.h>
#include <distorm.h>
#include "NtApi.h"
#include "base64.h"

//Macros:
#define strcmp_WS(wstr, char_ptr) (strcmp((char*)wstr.p, char_ptr) == 0)
#define WS_TO_STRING(wstr) (std::string((char*)wstr.p))
#define INSPECT_SIZE (0xF)
#define INST_MAXIMUM_SIZE (15) /* From: Distorm -> x86defs.h */
template NktIdList;
const char* RelativeInstructions[3] = { "e9", "e8", "eb" };

//Helpers:
BOOL IsInstructionRelative(const std::string& inst);
INT_PTR RawCallFunction(LPCVOID f, NktCallingConvention cc, const NktCallContext&, SIZE_T szBytes, SIZE_T cleanSize);

///
/// Utils::DbgPrintRegisters
///
void NktUtils::DbgPrintRegisters(NktRegisters* reg)
{
	char buffer[256] = {0};
	sprintf_s(buffer, "Registers - tid: %d# ", NktNtApi::GetCurrentThreadId());
	OutputDebugStringA(buffer);
	char* names[] = { "lastError", "EFLAGS", "EDI", "ESI", "EBP", "ESP", "EBX", "EDX", "ECX", "EAX" };
	for (int i = 0; i < sizeof(names)/sizeof(names[0]); ++i)
	{
		sprintf_s(buffer, "%s: %X -", names[i], reg[i]);
		OutputDebugStringA(buffer);
	}
	OutputDebugStringA("\n");
}


///
/// Utils::GetMinJmpSize
///
const SIZE_T NktUtils::GetMinJmpSize(const DV_PTR code, SIZE_T jmp_size)
{
	SIZE_T count = 0;
	_OffsetType offset = 0;
	_DecodeType dt = Decode32Bits;
	_DecodedInst* decodedInstructions = new _DecodedInst[INSPECT_SIZE];
	unsigned int dc = 0;

	//Decode instructions
	distorm_decode(offset, (const unsigned char*)code, INSPECT_SIZE, dt, decodedInstructions, INSPECT_SIZE, &dc);

	bool seek_min = true;
	unsigned int i = 0;
	while (seek_min)
	{
		count += decodedInstructions[i].size;
		i++;
		seek_min = (count < jmp_size) && (i < dc);
	}

	delete[] decodedInstructions;
	return count;
}

///
/// Utils::SimpleCallFunction
///
INT_PTR NktUtils::SimpleCallFunction(const NktFunctionWrapper& f, LPCVOID pms, SIZE_T szBytes, LPCVOID pThis)
{
	NktCallingConvention cc = static_cast<NktCallingConvention>(f.GetCallConvention());
	NktRegisters registers = {0};
	NktCallContext context;
	PINT_PTR params = (pThis && cc == thiscall_)? (PINT_PTR)alloca(szBytes+sizeof(pThis)) : (PINT_PTR)pms;
	context.regs = &registers;
	context.tag = 0;

	//Arrange parameters for call
	switch (cc)
	{
		case thiscall_:
		{
			if (pThis)
			{
				params[0] = (INT_PTR)pThis;
				memcpy((params+1), pms, szBytes);
			}
			break;
		}
		//This pointer on ECX
		case thiscall_ms_:
		{
			if (pThis)
			{
				registers.ECX = (INT_PTR)pThis;
			}
			else
			{
				_ASSERT(szBytes >= REGISTER_SIZE);
				registers.ECX = params[0];
				++params;
				szBytes -= REGISTER_SIZE;
			}
			break;
		}
		//First and second param on ECX & EDX
		case fastcall_:
		{
			int dec = 0;
			PINT_PTR regs[] = { &registers.ECX, &registers.EDX };
			while (dec < 2 && szBytes >= REGISTER_SIZE)
			{
				*regs[dec] = params[0];
				++params;
				szBytes -= REGISTER_SIZE;
			}
			break;
		}
	}

	context.pms = params;
	return RawCallFunction(
		f.GetAddress(),
		static_cast<NktCallingConvention>(f.GetCallConvention()),
		context, 
		f.GetStackParamSize(), 
		f.GetCallerCleanSize());
}

///
/// Utils::CallFunction
///
void NktUtils::CallFunction(const NktFunctionWrapper& fw, NktRegisters* registers, const void* params, SIZE_T szBytes)
{
	NktCallingConvention cc = static_cast<NktCallingConvention>(fw.GetCallConvention());
	NktCallContext context;
	context.pms = params;
	context.regs = registers;
	context.tag = 0;
	RawCallFunction(fw.GetAddress(), cc, context, fw.GetStackParamSize(), fw.GetCallerCleanSize());
}

///
/// Utils::UpdateIndirections
///
void NktUtils::UpdateIndirections(NktVirtualCode& code, const unsigned int prev_base, const unsigned int new_base)
{
	SIZE_T count = 0;
	_OffsetType offtype = 0;
	_DecodeType dt = Decode32Bits;
	SIZE_T codeSize = code.Pivot();
	SIZE_T maxInst = (codeSize < INST_MAXIMUM_SIZE)? INST_MAXIMUM_SIZE : codeSize;
	_DecodedInst* decodedInstructions =  (_DecodedInst*)alloca(maxInst * sizeof(_DecodedInst));
	unsigned int dc = 0;

	//Decode instructions:
	_DecodeResult res = distorm_decode(offtype, (unsigned char*)code.GetAddress(), (int)codeSize, dt, decodedInstructions, (unsigned int)maxInst, &dc);
	_ASSERT(dc);

	//Translate:
	DV_PTR diff = 0;
	for (unsigned int i = 0; i < dc; i++)
	{
		_DecodedInst& di = decodedInstructions[i];
		if (IsInstructionRelative(WS_TO_STRING(di.instructionHex)))
		{
			unsigned int opAt = (unsigned int)(di.offset + (di.size - sizeof(void*)));
			code.RawRead(opAt, (unsigned int*)&diff);
			DV_PTR addrDest = prev_base + diff;
			code.RawWrite(opAt, addrDest - new_base);
		}
	}
}

///
/// Utils::WriteMem
///
BOOL NktUtils::WriteMem(const NktVirtualCode& code, void* dest, DWORD pid)
{
	DWORD oldProt = 0;
	BOOL ret;
	ret = VirtualProtect(dest, code.GetSize(), PAGE_EXECUTE_READWRITE, &oldProt);
	if (ret)
	{
		HANDLE hproc = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, pid);
		ret = WriteProcessMemory(hproc, dest, (LPVOID)code.GetAddress(), code.Pivot(), NULL);
		VirtualProtect(dest, code.GetSize(), oldProt, NULL);
		::CloseHandle(hproc);
	}
	return ret;
}

///
/// Utils::SuspendThreads
///
void NktUtils::SuspendThreads(NktSegmentRange range, OUT NktIdList& suspended_threads, DWORD process_id)
{
	DWORD curr_thread_id = GetCurrentThreadId();
	HANDLE hSnapshot = NULL;
	bool suspending = true;
	THREADENTRY32 thread_entry;
	CONTEXT thread_context;
	ZeroMemory(&thread_entry, sizeof(THREADENTRY32));
	ZeroMemory(&thread_context, sizeof(CONTEXT));
	thread_entry.dwSize = sizeof(THREADENTRY32);
	thread_context.ContextFlags = CONTEXT_ALL;

	while (suspending)
	{
		suspending = false;

		//Get snapshot:
		hSnapshot =  CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, process_id);
		if(hSnapshot == INVALID_HANDLE_VALUE)
			throw E_HANDLE;

		//Loop threads:
		if (!Thread32First(hSnapshot, &thread_entry))
			throw E_FAIL;
		do
		{
			//Skip for this thread and other process' threads
			if ((thread_entry.th32OwnerProcessID != process_id) ||
				(thread_entry.th32ThreadID == curr_thread_id) )
				continue;

			//Skip if we already suspended this thread:
			if (suspended_threads.contains(thread_entry.th32ThreadID))
				continue;

			//Open thread for context and suspend:
			HANDLE hThread = OpenThread(THREAD_GET_CONTEXT | THREAD_SUSPEND_RESUME, FALSE, thread_entry.th32ThreadID);

			//Suspend the thread:
			NktNtApi::SuspendThread(hThread);

			//Give thread processor until it leaves our segment:
			GetThreadContext(hThread, &thread_context);
			while (range.Contains((void*)thread_context.Eip) == 0)
			{
				ResumeThread(hThread);

				Sleep(0);
				NktNtApi::SuspendThread(hThread);

				GetThreadContext(hThread, &thread_context);
			}

			suspended_threads.push_back(thread_entry.th32ThreadID);
			CloseHandle(hThread);
			suspending = true;
		}
		while(Thread32Next(hSnapshot, &thread_entry));

		//Release snapshot:
		CloseHandle(hSnapshot);
	}
}

///
/// Utils::ResumeThreads
///
void NktUtils::ResumeThreads(IN NktIdList& suspended_threads)
{
	for (NktIdList::iterator it = suspended_threads.begin(); it != suspended_threads.end(); ++it)
	{
		DWORD thread_id = *it;
		HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, thread_id);
		ResumeThread(hThread);
		::CloseHandle(hThread);//CloseHandle(hThread);
	}
}

///
/// Utils::CreateCall
///
void NktUtils::CreateCall(const NktFunctionWrapper& fnc, OUT NktVirtualCode& code, LPCVOID pThis, bool useJmp)
{
	int cc = fnc.GetCallConvention();
	USHORT cleanSize = fnc.GetCallerCleanSize();

	//Params:
	switch (cc)
	{
		//First param on Stack:
		case stdcall_:
		case cdecl_:
		{
			if (!pThis) break;
		}
		case thiscall_:
		{
			code.Append(PUSH_INM, (UINT)pThis);
			break;
		}
		//First param on ECX:
		case fastcall_:
		{
			if (!pThis)	break;
		}
		case thiscall_ms_:
		{
			code.Append(MOV_ECX, (UINT)pThis);
			break;
		}
		default:
			NODEFAULT;
	}

	//Call:
	DV_PTR fncAddr = (DV_PTR)fnc.GetAddress();
	(!useJmp)? code.AppendCall(fncAddr) : code.AppendJump(fncAddr);
	//Clean:
	NktUtils::CreateClean(fnc, cleanSize, code);
}
NktVirtualCode NktUtils::CreateCall(const NktFunctionWrapper& fnc, LPCVOID pThis, bool useJmp)
{
	NktVirtualCode code;
	CreateCall(fnc, code, pThis, useJmp);
	return code;
}

///
/// Utils::CreateClean
///
void NktUtils::CreateClean(const NktFunctionWrapper& fnc, SIZE_T size, OUT NktVirtualCode& code)
{
	int cc = fnc.GetCallConvention();
	switch(cc)
	{
		//Caller cleans
		case syscall_:
		case thiscall_:
		case cdecl_:
		{
			code.Append(ADD_ESP, (UCHAR)size);
			break;
		}
		//Callee cleans
		case thiscall_ms_:
		case stdcall_:
		case pascal_:
		case safecall_:
		case fastcall_:
			break;
	}
}

///
/// Utils::AppendJmp
///
void NktUtils::AppendUpdatedJmp(NktVirtualCode& code, const void* fnc)
{
	//Append overridden memory:
	SIZE_T size = code.Pivot();
	NktUtils::UpdateIndirections(code, (unsigned int)fnc, code.GetAddress());

	//Append jump to original function:
	code.AppendJump((DV_PTR)fnc + size);
}

///
/// Tools::GetThreadString
///
#define EV_BASE_NAME (L"\\BaseNamedObjects\\DV_")
int __fastcall NktUtils::GetThreadString(WCHAR* buffer)
{
	_ASSERT(buffer);
	DWORD tid = NktNtApi::GetCurrentThreadId();
	wcscpy(buffer, EV_BASE_NAME);
	int length = wcslen(EV_BASE_NAME);
	length += base64_encode((unsigned char*)&tid, &buffer[length], sizeof(tid));
	buffer[length++] = '\0';
	return length;
}

/************************************************************************/
/* Helpers                                                              */
/************************************************************************/

///
/// IsInstructionRelative
///
BOOL IsInstructionRelative(const std::string& inst)
{
	BOOL ret = FALSE;
	for (int i = 0; i < sizeof(RelativeInstructions)/sizeof(RelativeInstructions[0]) && !ret; ++i)
	{
		ret = inst.find(RelativeInstructions[i]) != std::string::npos;
	}
	return ret;
}

///
/// RawCallFunction
///
INT_PTR RawCallFunction(LPCVOID f, NktCallingConvention cc, const NktCallContext& ctx, SIZE_T stackBytes, SIZE_T cleanSize)
{
	_ASSERT(f && ctx.regs);
	PINT_PTR rECX, rEDX, rEAX, rEFL, rError, params;
	NktRegisters* registers = ctx.regs;
	params = (PINT_PTR)ctx.pms;
	rEAX = &registers->EAX;
	rECX = &registers->ECX;
	rEDX = &registers->EDX;
	rEFL  = &registers->EFLAGS;
	rError = &registers->lastError;

	_asm
	{
		//prelude:
		push ecx
		push esi
		push edi

		//Load params:
		mov esi, [params]
		test esi, esi
		jz _asm_stack_loaded
		mov ecx, [stackBytes]
		sub esp, ecx
		shr ecx, 2
		mov esi, [params]
		mov edi, esp
		rep movsd
_asm_stack_loaded:

		//Load ECX:
		mov esi, [rECX]
		mov ecx, [esi]

		//Load EDX:
		mov esi, [rEDX]
		mov edx, [esi]

		//Restore Thread Last Error:
		mov esi, rError
		mov esi, [esi]
		mov edi, fs:18h
		mov [edi+34h], esi

		//Call
		mov eax, [f]
		call eax

		//Store EAX(return):
		mov edi, rEAX
		mov [edi], eax

		//Store FLAGS(return):
		pushfd
		pop eax
		mov edi, rEFL
		mov [edi], eax

		//Store Thread Last Error:
		mov eax, fs:18h
		mov eax, [eax+34h]
		mov edi, rError
		mov [edi], eax

_asm_epilogue:
		//Clear stacked params
		// *add 0 to esp seems to be a better choice than testing if it's necessary on every case...
		add esp, cleanSize

		//Restore registers and return
		pop edi
		pop esi
		pop ecx
		//ret
	}

}