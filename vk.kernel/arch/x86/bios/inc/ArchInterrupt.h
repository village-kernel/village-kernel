//###########################################################################
// ArchInterrupt.h
// Declarations of the functions that manage arch interrupt
//
// $Copyright: Copyright (C) village
//###########################################################################
#ifndef __ARCH_INTERRUPT_H__
#define __ARCH_INTERRUPT_H__

#include "Registers.h"


/// @brief ArchInterrupt
class ArchInterrupt
{
public:
	//Static constants
	static const uint32_t isr_num = 256;
private:
	//Structures
	struct IdtGate
	{
		uint16_t lowOffset;  /* Lower 16 bits of handler function address */
		uint16_t sel;        /* Kernel segment selector */
		uint8_t  rsvd;
		/* First byte
		* Bit 7: "Interrupt is present"
		* Bits 6-5: Privilege level of caller (0=kernel..3=user)
		* Bit 4: Set to 0 for interrupt gates
		* Bits 3-0: bits 1110 = decimal 14 = "32 bit interrupt gate" */
		uint8_t  flags; 
		uint16_t highOffset; /* Higher 16 bits of handler function address */

		IdtGate() :
			lowOffset(0),
			sel(0),
			rsvd(0),
			flags(0),
			highOffset(0)
		{}
	} __attribute__((packed));

	struct IdtRegister
	{
		uint16_t limit;
		uint32_t base;
		
		IdtRegister() :
			limit(0),
			base(0)
		{}
	} __attribute__((packed));

	//Static constants
	static const uint32_t kernel_code_segment = 8;
	static const uint32_t idt_entires = isr_num;

	//Members
	IdtGate idt[idt_entires];
	IdtRegister idtReg;

	//Methods
	void SetIdtGate(int irq, uint32_t handler);
	void SetIdt();
	void RemapPIC();
public:
	//Methods
	void Setup();
	void Exit();
	uint16_t GetPicIrqReg(int ocw3);
	uint16_t GetPicIrr();
	uint16_t GetPicIsr();
};

#endif //!__ARCH_INTERRUPT_H__
