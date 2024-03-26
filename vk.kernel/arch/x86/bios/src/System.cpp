//###########################################################################
// System.h
// A mostly static class containing system wide functions.
//
// $Copyright: Copyright (C) village
//###########################################################################
#include "System.h"
#include "Kernel.h"
#include "Hardware.h"


/// @brief Constructor
ConcreteSystem::ConcreteSystem()
	:sysTicks(0)
{
}


/// @brief Destructor
ConcreteSystem::~ConcreteSystem()
{
}


/// @brief System Setup
void ConcreteSystem::Setup()
{
	//Set interrupt handler
	kernel->interrupt.SetISR(IRQ_Systick, (Method)&ConcreteSystem::SysTickHandler, this);

	//Configure clock
	ConfigureClock();
}


/// @brief System Exit
void ConcreteSystem::Exit()
{

}


/// @brief Configure clock
void ConcreteSystem::ConfigureClock()
{
	//Reset systicks
	sysTicks = 0;

	//Get the PIT value: hardware clock at 1193182 Hz
	uint32_t freq = 1000; //1000hz, 1ms
	uint32_t divider = 1193182 / freq;
	uint8_t low  = low_8(divider);
	uint8_t high = high_8(divider);

	//Send the command
	PortByteOut(TIMER_CMD, 0x36); //Command port
	PortByteOut(TIMER_CH0, low);
	PortByteOut(TIMER_CH0, high);
}


/// @brief Delays for a specified number of milliseconds.
void ConcreteSystem::DelayMs(uint32_t millis)
{
	uint32_t delayStart = sysTicks;
	uint32_t delayCycles = millis;
	while ((sysTicks - delayStart) < delayCycles);
}


/// @brief Reset x86 core
void ConcreteSystem::Reboot()
{
	uint64_t null_idtr = 0;
	__asm volatile ("lidt %0; int3" :: "m" (null_idtr));

	//Loop with interrupt off if it doesn't work
	__asm volatile ("cli");
	while (1) { __asm volatile ("hlt"); }
}


/// @brief Enables IRQ interrupts
void ConcreteSystem::EnableIRQ() { __asm volatile("sti"); }


/// @brief Disables IRQ interrupts
void ConcreteSystem::DisableIRQ() { __asm volatile("cli"); }


/// @brief Get system clock count
/// @return 
uint32_t ConcreteSystem::GetSysClkCounts() { return sysTicks; }


/// @brief System clock counter
void ConcreteSystem::SysTickCounter() { sysTicks++; }


/// @brief System clock handler
void ConcreteSystem::SysTickHandler() { sysTicks++; }

