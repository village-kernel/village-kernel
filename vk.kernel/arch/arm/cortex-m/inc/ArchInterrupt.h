//###########################################################################
// ArchInterrupt.h
// Declarations of the functions that manage interrupt
//
// $Copyright: Copyright (C) village
//###########################################################################
#ifndef __ARCH_INTERRUPT_H__
#define __ARCH_INTERRUPT_H__

#include "Kernel.h"


/// @brief Interrupt
class ArchInterrupt
{
public:
	//Static constants
	static const uint32_t isr_num = 256;
	static const uint32_t rsvd_isr_size = 16;
private:
	//Methods
	void ConfigVectorTable(uint32_t vector);
public:
	//Methods
	void Setup();
	void Exit();
};

#endif //!__INTERRUPT_H__
