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
#ifndef _ASMCODE_H_
#define _ASMCODE_H_

#include "types.h"

#define SIZEOF_PUSHAD	(0x20)
#define TEB_OFFSET		((unsigned int)0x18)
#define TEB_LASTERROR	((UCHAR)0x34)

#define REG_EAX			((asmcode)0x30)
#define REG_EBX			((asmcode)0x33)
#define REG_ECX			((asmcode)0x31)
#define REG_EDX			((asmcode)0x32)
#define REG_ESI			((asmcode)0x36)
#define REG_EDI			((asmcode)0x37)
#define REG_ESP			((asmcode)0x24)
#define REG_ESP_PTR		((asmcodew)0x2404)
#define REG_EBP			((asmcodew)0x0075)

#define ADD_EAX			((asmcodew)0xc083)
#define ADD_EBP			((asmcodew)0xc583)
#define ADD_ESP			((asmcodew)0xc483)
#define ADD_EDX			((asmcodew)0xc283)
#define ADD_EAX_EDI_PTR	((asmcodew)0x0703)
#define CALL			((asmcode)0xe8)
#define CMP_EAX			((asmcodew)0x3880)
#define CMP_EAX_INM		((asmcodew)0xf883)
#define INT3			((asmcode)0xcc)
#define NOP				((asmcode)0x90)
#define JE				((asmcode)0x74)
#define JNE				((asmcode)0x75)
#define JMP				((asmcode)0xe9)
#define LEA				((asmcode)0x8d)
#define LOCK			((asmcode)0xf0)
#define MOV_EAX			((asmcode)0xb8)
#define MOV_EAX_EDI_PTR	((asmcodew)0x078b)
#define MOV_EAX_ESP		((asmcodew)0xc48b)
#define MOV_EAX_FS		((asmcodew)0xA164)
#define MOV_EAX_PTR		((asmcode)0xa1)
#define MOV_EAX_PTR_EDX ((asmcodew)0x1089)
#define MOV_EBP_ESP		((asmcodew)0xec8b)
#define MOV_ECX			((asmcode)0xb9)
#define MOV_ECX_EAX		((asmcodew)0xc88b)
#define MOV_ECX_REG_PTR	((asmcodew)0x0c8b)
#define MOV_ECX_ESP		((asmcodew)0xcc8b)
#define MOV_EDX_ESP		((asmcodew)0xd48b)
#define MOV_EDI			((asmcode)0xbf)
#define MOV_EDI_ESP		((asmcodew)0xfc8b)
#define MOV_EDI_PTR		((asmcodew)0x3d8b)
#define MOV_EDI_PTR_EAX	((asmcodew)0x0789)
#define MOV_ESP			((asmcode)0xbc)
#define MOV_ESP_EAX		((asmcodew)0xe08b)
#define MOV_ESP_PTR		((asmcodew)0x2404)
#define MOV_REG_PTR_INM ((asmcode)0xc7)
#define POPAD			((asmcode)0x61)
#define POP_EAX			((asmcode)0x58)
#define POP_ECX			((asmcode)0x59)
#define POP_EDX			((asmcode)0x5a)
#define POP_EBP			((asmcode)0x5d)
#define POP_EDI			((asmcode)0x5F)
#define POP_ESP			((asmcode)0x5c)
#define POPFD			((asmcode)0x9d)
#define PUSHAD			((asmcode)0x60)
#define PUSH_EAX		((asmcode)0x50)
#define PUSH_EBP		((asmcode)0x55)
#define PUSH_ECX		((asmcode)0x51)
#define PUSH_EDI		((asmcode)0x57)
#define PUSH_EDI_PTR	((asmcodew)0x37ff)
#define PUSH_EDX		((asmcode)0x52)
#define PUSH_ESP		((asmcode)0x54)
#define PUSH_INM		((asmcode)0x68)
#define PUSH_PTR		((asmcode)0xff)
#define PUSHFD			((asmcode)0x9c)
#define RET				((asmcode)0xc3)
#define RET_CLEAN		((asmcode)0xc2)
#define SUB_EAX			((asmcodew)0xe883)
#define SUB_ESP			((asmcodew)0xec81)
// increment the content pointed by the inmediate
#define INC_MEM			((asmcodew)0x05ff)
// decrement the content pointed by the inmediate
#define DEC_MEM			((asmcodew)0x0dff)

#pragma pack(push, 1) //Prevent compiler from reordering the struct

//Single operand class
template <class OP_T>
class NktAsmBase
{
public:
	OP_T opcode;
};

//Template Code + Operand class:
template <class OP_T, class PARAM_T, OP_T code>
class NktAsmOp : public NktAsmBase<OP_T>
{
public:
	NktAsmOp() { this->opcode = code; }
public:
	PARAM_T param;
};

//OP: byte
template <class PARAM_T, asmcode code>
class NktAsmOpByte : public NktAsmOp<asmcode, PARAM_T, code> {};

//OP: word
template <class PARAM_T, asmcodew code>
class NktAsmOpWord : public NktAsmOp<asmcodew, PARAM_T, code> {};

//asm_code: structure to contain asm code.
class NktAsmCode
{
public:
	NktAsmCode(const unsigned int size);
	NktAsmCode(const NktAsmCode& cp);
	~NktAsmCode();
	DV_PTR GetAddress() const;
	SIZE_T GetSize() const;

protected:
	unsigned int _size;
	asmcode* _code;

private:
	size_t* _refCount;
};
#pragma pack(pop)


#endif //_ASMCODE_H_