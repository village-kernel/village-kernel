//###########################################################################
// Scheduler.cpp
// Definitions of the functions that manage thread scheduler
//
// $Copyright: Copyright (C) village
//###########################################################################
#include "Scheduler.h"
#include "Hardware.h"


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
	//Set the PendSV interrupt handler
	kernel->interrupt.SetISR(IRQ_PendSV, (Method)&ConcreteScheduler::PendSVHandler, this);

	//Append the systick interrupt handler
	kernel->interrupt.AppendISR(IRQ_Systick, (Method)&ConcreteScheduler::SysTickHandler, this);
}


/// @brief Scheduler Exit
void ConcreteScheduler::Exit()
{
	//Append the systick interrupt handler
	kernel->interrupt.RemoveISR(IRQ_Systick, (Method)&ConcreteScheduler::SysTickHandler, this);

	//Set the PendSV interrupt handler
	kernel->interrupt.ClearISR(IRQ_PendSV);
}


/// @brief Start scheduler
void ConcreteScheduler::Start()
{
	//Clear start schedule flag
	isStartSchedule = false;

	//Get frist task psp
	uint32_t psp = kernel->thread.GetTaskPSP();

	//Set frist task esp
	__asm volatile("movl %0, %%esp" : "=r"(psp));

	//Set start schedule flag
	isStartSchedule = true;

	//Set interrupt flag
	__asm volatile("sti");

	//Execute thread idle task
	kernel->thread.IdleTask();
}


/// @brief Rescheduler task
/// @param access scheduler access
void ConcreteScheduler::Sched(ConcreteScheduler::Access access)
{
	if (false == isStartSchedule) return;

	//Trigger PendSV directly
	__asm volatile("int $31");
}


/// @brief PendSV handler
void __attribute__((naked)) ConcreteScheduler::PendSVHandler()
{
	uint32_t psp = 0;

	//Push old task registers
	__asm volatile("pushl %ebp");
	__asm volatile("pushl %ebx");
	__asm volatile("pushl %esi");
	__asm volatile("pushl %edi");
	__asm volatile("movl %%esp, %0" : "=r"(psp));

	//Save old task psp
	kernel->thread.SaveTaskPSP(psp);

	//Select next task
	kernel->thread.SelectNextTask();

	//Get new task psp
	psp = kernel->thread.GetTaskPSP();

	//Set new task esp
	__asm volatile("movl %0, %%esp" : "=r"(psp));

	//Pop new task registers
	__asm volatile("popl %edi");
	__asm volatile("popl %esi");
	__asm volatile("popl %ebx");
	__asm volatile("popl %ebp");
	__asm volatile("sti");
	__asm volatile("ret");
}


/// @brief SysTick handler
void ConcreteScheduler::SysTickHandler()
{
	Sched(ConcreteScheduler::Privileged);
}
