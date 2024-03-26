//###########################################################################
// Interrupt.h
// Declarations of the functions that manage interrupt
//
// $Copyright: Copyright (C) village
//###########################################################################
#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

#include "Kernel.h"
#include "List.h"
#include "Cast.h"
#include "Exception.h"
#include "ArchInterrupt.h"


/// @brief ConcreteInterrupt
class ConcreteInterrupt : public Interrupt
{
private:
	//Structures
	struct Isr
	{
		int32_t     irq;
		Function    func;
		void*       user;
		void*       args;

		Isr(int32_t irq = 0, Function func = NULL, void* user = NULL, void* args = NULL)
			:irq(irq),
			func(func),
			user(user),
			args(args)
		{}
	};

	//Static constants
	static const uint32_t warning_times = 10;

	//Members
	Exception     exception;
	ArchInterrupt archInterrupt;
	Debug*        debug;
	uint8_t       warnings[ArchInterrupt::isr_num] = { 0 };
	List<Isr*>    isrTabs[ArchInterrupt::isr_num];
public:
	//Methods
	ConcreteInterrupt();
	~ConcreteInterrupt();
	void Setup();
	void Exit();
	int SetISR(int irq, Function func, void* user = NULL, void* args = NULL);
	int SetISR(int irq, Method method, Class* user, void* args = NULL);
	int AppendISR(int irq, Function func, void* user = NULL, void* args = NULL);
	int AppendISR(int irq, Method method, Class* user, void* args = NULL);
	bool RemoveISR(int irq, Function func, void* user = NULL, void* args = NULL);
	bool RemoveISR(int irq, Method method, Class* user, void* args = NULL);
	void ClearISR(int irq);
	void Handler(int irq);
};

#endif //!__INTERRUPT_H__
