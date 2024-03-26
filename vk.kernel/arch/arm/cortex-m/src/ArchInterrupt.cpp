//###########################################################################
// ArchInterrupt.cpp
// Definitions of the functions that manage arch interrupt
//
// $Copyright: Copyright (C) village
//###########################################################################
#include "ArchInterrupt.h"
#include "HalHeaders.h"


/// @brief Declarations
extern "C" void Stub_Handler();
extern "C" void Default_Handler();


/// @brief Interrupt Setup
void ArchInterrupt::Setup()
{
	//Symbol defined in the linker script
	extern void *_sivector, *_svector, *_evector;

	//Copy isr vector from sivector
	void **pSource = &_sivector, **pDest = &_svector;
	for (; pDest != &_evector; pSource++, pDest++)
		*pDest = *pSource;

	//Calculate the size of isr vector
	uint32_t isrSizes = ((uint32_t)&_evector - (uint32_t)&_svector) >> 2;
	isrSizes = isrSizes - rsvd_isr_size;

	//Calculate the address of usr isr vectors
	uint32_t* vectors = (uint32_t*)&_svector + rsvd_isr_size;

	//Set the new isr handler
	for (uint32_t i = 0; i < isrSizes; i++)
	{
		//Change the origin handler into isr table
		if (vectors[i] != (uint32_t)&(Default_Handler) + 1)
		{
			kernel->interrupt.SetISR(i, (Function)vectors[i], NULL, NULL);
		}

		vectors[i] = (uint32_t)&Stub_Handler;
	}

	//Relocation isr vecotr table
	ConfigVectorTable((uint32_t)&_svector);
}


/// @brief Exit
void ArchInterrupt::Exit()
{

}


/// @brief System config vector table, this value must be a multiple of 0x200.
void ArchInterrupt::ConfigVectorTable(uint32_t vector)
{
	SCB->VTOR = vector;
}


/// @brief Stub handler
extern "C"  __attribute__ ((naked)) void Stub_Handler()
{
	uint32_t irq = 0;

	//Save LR back to main, must do this firstly
	__asm volatile("push {lr}");

	//Gets the ipsr value
	__asm volatile("mrs %0, ipsr" : "=r"(irq));

	//Go to interrupt handler
	kernel->interrupt.Handler(irq - ArchInterrupt::rsvd_isr_size);

	//Exit
	__asm volatile("pop {lr}");
	__asm volatile("bx lr");
}
