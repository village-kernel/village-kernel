//###########################################################################
// Exception.cpp
// Definitions of the functions that manage exception
//
// $Copyright: Copyright (C) village
//###########################################################################
#include "Exception.h"
#include "Kernel.h"
#include "HalHeaders.h"


/// @brief Constructor
Exception::Exception()
{
}


/// @brief Deconstructor
Exception::~Exception()
{
}


/// @brief Exception Setup
void Exception::Setup()
{
	
}


/// @brief Exception Exit
void Exception::Exit()
{

}


/// @brief Output stacked info
/// @param hardfault_args stack pointer
extern "C" void stacked_info(unsigned int * hardfault_args)
{
	volatile uint32_t stacked_r0 = ((uint32_t)hardfault_args[0]);
	volatile uint32_t stacked_r1 = ((uint32_t)hardfault_args[1]);
	volatile uint32_t stacked_r2 = ((uint32_t)hardfault_args[2]);
	volatile uint32_t stacked_r3 = ((uint32_t)hardfault_args[3]);

	volatile uint32_t stacked_r12 = ((uint32_t)hardfault_args[4]);
	volatile uint32_t stacked_lr  = ((uint32_t)hardfault_args[5]);
	volatile uint32_t stacked_pc  = ((uint32_t)hardfault_args[6]);
	volatile uint32_t stacked_psr = ((uint32_t)hardfault_args[7]);

	kernel->debug.Output(Debug::_Lv5, "Hard_Fault_Handler:");
	kernel->debug.Output(Debug::_Lv5, "r0:   0x%08lx", stacked_r0);
	kernel->debug.Output(Debug::_Lv5, "r1:   0x%08lx", stacked_r1);
	kernel->debug.Output(Debug::_Lv5, "r2:   0x%08lx", stacked_r2);
	kernel->debug.Output(Debug::_Lv5, "r3:   0x%08lx", stacked_r3);
	kernel->debug.Output(Debug::_Lv5, "r12:  0x%08lx", stacked_r12);
	kernel->debug.Output(Debug::_Lv5, "lr:   0x%08lx", stacked_lr);
	kernel->debug.Output(Debug::_Lv5, "pc:   0x%08lx", stacked_pc);
	kernel->debug.Output(Debug::_Lv5, "xpsr: 0x%08lx", stacked_psr);

	while (1);
}


/// @brief HardFault_Handler
/// @param  
extern "C" void HardFault_Handler(void)
{
	__asm volatile("tst lr, #4");      // check LR to know which stack is used
	__asm volatile("ite eq");          // 2 next instructions are conditional
	__asm volatile("mrseq r0, msp");   // save MSP if bit 2 is 0
	__asm volatile("mrsne r0, psp");   // save PSP if bit 2 is 1
	__asm volatile("b stacked_info");  // pass R0 as the argument
}
