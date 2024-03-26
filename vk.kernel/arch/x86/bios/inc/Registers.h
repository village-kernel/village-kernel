//###########################################################################
// Registers-x86.h
// Declarations of the i386 registers
//
// $Copyright: Copyright (C) village
//###########################################################################
#ifndef __REGISTERS_X86_H__
#define __REGISTERS_X86_H__

#include "stdint.h"

struct Registers
{
	//Data segment selector
	uint32_t ds;        
	
	//Pushed by pusha.
	uint32_t edi;
	uint32_t esi;
	uint32_t ebp;
	uint32_t esp;
	uint32_t ebx;
	uint32_t edx;
	uint32_t ecx;
	uint32_t eax;

	//Interrupt number and error code
	uint32_t irq;
	uint32_t err;
	
	//Pushed by the processor automatically
	uint32_t eip;
	uint32_t cs;
	uint32_t eflags;
	uint32_t psp;
	uint32_t ss;

	Registers
	(
		uint32_t eip = 0,
		uint32_t eax = 0,
		uint32_t ecx = 0,
		uint32_t edx = 0,
		uint32_t ebx = 0,
		uint32_t esp = 0,
		uint32_t ebp = 0,
		uint32_t esi = 0,
		uint32_t edi = 0
	):
		ds(0),
		edi(edi),
		esi(esi),
		ebp(ebp),
		esp(esp),
		ebx(ebx),
		edx(edx),
		ecx(ecx),
		eax(eax),
		irq(0),
		err(0),
		eip(eip),
		cs(0),
		eflags(0x00000200u),
		psp(0),
		ss(0)
	{}
} __attribute__((packed));

struct TaskContext
{
	uint32_t edi;
	uint32_t esi;
	uint32_t ebx;
	uint32_t ebp;
	uint32_t eip;

	uint32_t ret;
	uint32_t arg0;
	uint32_t arg1;
	uint32_t arg2;
	uint32_t arg3;

	TaskContext
	(
		uint32_t eip  = 0,
		uint32_t arg0 = 0,
		uint32_t arg1 = 0,
		uint32_t arg2 = 0,
		uint32_t arg3 = 0
	):
		edi(0),
		esi(0),
		ebx(0),
		ebp(0x2000000),
		eip(eip),
		ret(0),
		arg0(arg0),
		arg1(arg1),
		arg2(arg2),
		arg3(arg3)
	{}
} __attribute__((packed));

#endif //!__REGISTERS_X86_H__
