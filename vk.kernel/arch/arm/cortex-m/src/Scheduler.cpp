//###########################################################################
// Scheduler.cpp
// Definitions of the functions that manage thread scheduler
//
// $Copyright: Copyright (C) village
//###########################################################################
#include "Scheduler.h"
#include "Kernel.h"
#include "System.h"
#include "HalHeaders.h"


/// @brief Constructor
ConcreteScheduler::ConcreteScheduler()
	:isStartSchedule(false)
{
}


/// @brief Destructor
ConcreteScheduler::~ConcreteScheduler()
{
}


/// @brief Scheduler Setup
void ConcreteScheduler::Setup()
{
	
}


/// @brief Scheduler Exit
void ConcreteScheduler::Exit()
{

}


/// @brief Start scheduler
void ConcreteScheduler::Start()
{
	//Clear start schedule flag
	isStartSchedule = false;

	//Get frist task psp
	uint32_t psp = kernel->thread.GetTaskPSP();

	//Set frist task esp
	__asm volatile("msr psp, %0" : "=r"(psp));

	//Change to use PSP, set bit[1] SPSEL
	__asm volatile("mrs r0, control");
	__asm volatile("orr r0, r0, #2");
	__asm volatile("msr control, r0");

	//Move to Unprivileged level, Set bit[0] nPRIV
	__asm volatile("mrs r0, control");
	__asm volatile("orr r0, r0, #1");
	__asm volatile("msr control, r0");

	//Set start schedule flag
	isStartSchedule = true;

	//Execute thread
	kernel->thread->IdleTask();
}


/// @brief Rescheduler task
/// @param access scheduler access
void ConcreteScheduler::Sched(ConcreteScheduler::Access access)
{
	if (false == isStartSchedule) return;

	if (Access::Privileged == access)
	{
		// trigger PendSV directly
		SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
	}
	else
	{
		// call Supervisor exception to get Privileged access
		__asm volatile("SVC #255");
	}
}


/// @brief Interrupt handlers
extern "C" 
{
	/// @brief Systick handler
	void SysTick_Handler(void)
	{
		kernel->system.SysTickCounter();
		kernel->scheduler.Sched(ConcreteScheduler::Privileged);
	}


	/// @brief PendSV_Handler
	/// @param  
	void __attribute__ ((naked)) PendSV_Handler(void)
	{
		uint32_t psp = 0;

		//Save LR back to main, must do this firstly
		__asm volatile("push {lr}");

		//Gets the current psp
		__asm volatile("mrs %0, psp" : "=r"(psp));

		//Save R4 to R11 to psp frame stack
		__asm volatile("stmdb %0!, {r4-r11}" : "=r"(psp));

		//Save current value of psp
		kernel->thread.SaveTaskPSP(psp);

		//Select next task
		kernel->thread.SelectNextTask();

		//Get its past psp value, return psp is in R0
		psp = kernel->thread.GetTaskPSP();

		//Retrieve R4-R11 from psp frame stack
		__asm volatile("ldmia %0!, {r4-r11}" : "=r"(psp));

		//Update psp
		__asm volatile("msr psp, %0" : "=r"(psp));

		//Exit
		__asm volatile("pop {lr}");
		__asm volatile("bx lr");
	}


	/// @brief Execute task requests
	/// @param sp stack pointer
	void TaskOperator(uint32_t* sp)
	{
		//Get the address of the instruction saved in PC
		uint8_t *pInstruction = (uint8_t*)(sp[6]);

		//Go back 2 bytes (16-bit opcode)
		pInstruction -= 2;

		//Get the opcode, in little endian
		uint8_t svcNumber = *pInstruction;

		switch(svcNumber)
		{
			case 0xFF:
				//Trigger PendSV
				SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
				break;
			default:
				break;
		}
	}


	/// @brief SVC_Handler
	/// @param  
	void __attribute__ ((naked)) SVC_Handler(void)
	{
		__asm volatile("tst lr, 4");        // check LR to know which stack is used
		__asm volatile("ite eq");           // 2 next instructions are conditional
		__asm volatile("mrseq r0, msp");    // save MSP if bit 2 is 0
		__asm volatile("mrsne r0, psp");    // save PSP if bit 2 is 1
		__asm volatile("b TaskOperator");   // pass R0 as the argument
	}
}
