//###########################################################################
// Interrupt.cpp
// Definitions of the functions that manage interrupt
//
// $Copyright: Copyright (C) village
//###########################################################################
#include "Interrupt.h"
#include "ArchInterrupt.h"
#include "Kernel.h"
#include "Debug.h"


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
	Debug*        debug;
	uint8_t       warnings[ArchInterrupt::isr_num] = { 0 };
	List<Isr*>    isrTabs[ArchInterrupt::isr_num];
public:
	/// @brief Constructor
	ConcreteInterrupt()
		:debug(NULL)
	{
	}


	/// @brief Destructor
	~ConcreteInterrupt()
	{
	}


	/// @brief Interrupt initialize
	void Initialize()
	{
		debug = (Debug*)kernel->modular->GetModule(ModuleID::_debug);
	}


	/// @brief Interrupt Set ISR, this will clean the isrTabs[irq]
	/// @param irq irq number
	/// @param func interupt function
	/// @param user interrupt user
	/// @param args interrupt args
	/// @return the number of the isr in isrTabs, return -1 when fail.
	int SetISR(int irq, Function func, void* user, void* args)
	{
		ClearISR(irq);
		return isrTabs[irq].Add(new Isr(irq, func, user, args));
	}


	/// @brief Interrupt Set ISR, this will clean the isrTabs[irq]
	/// @param irq irq number
	/// @param method interupt method
	/// @param user interrupt user
	/// @param args interrupt args
	/// @return the number of the isr in isrTabs, return -1 when fail.
	int SetISR(int irq, Method method, Class* user, void* args)
	{
		return SetISR(irq, union_cast<Function>(method), (void*)user, args);
	}


	/// @brief Interrupt append ISR
	/// @param irq irq number
	/// @param func interupt function
	/// @param user interrupt user
	/// @param args interrupt args
	/// @return the number of the isr in isrTabs, return -1 when fail.
	int AppendISR(int irq, Function func, void* user, void* args)
	{
		return isrTabs[irq].Add(new Isr(irq, func, user, args));
	}


	/// @brief Interrupt append ISR
	/// @param irq irq number
	/// @param method interupt method
	/// @param user interrupt user
	/// @param args interrupt args
	/// @return the number of the isr in isrTabs, return -1 when fail.
	int AppendISR(int irq, Method method, Class* user, void* args)
	{
		return AppendISR(irq, union_cast<Function>(method), (void*)user, args);
	}


	/// @brief Interrupt remove isr
	/// @param irq irq number
	/// @param func interrupt function
	/// @param user interrupt user
	/// @param args interrupt args
	/// @return Result::_OK / Result::_ERR
	int RemoveISR(int irq, Function func, void* user, void* args)
	{
		List<Isr*>* isrs = &isrTabs[irq];

		for (Isr* isr = isrs->Begin(); !isrs->IsEnd(); isr = isrs->Next())
		{
			if ((irq  == isr->irq ) &&
				(func == isr->func) &&
				(user == isr->user) &&
				(args == isr->args))
			{
				return (Result)isrs->Remove(isr, isrs->GetNid());
			}
		}

		return _ERR;
	}


	/// @brief Interrupt remove isr
	/// @param irq irq number
	/// @param method interrupt method
	/// @param user interrupt user
	/// @param args interrupt args
	/// @return Result::_OK / Result::_ERR
	int RemoveISR(int irq, Method method, Class* user, void* args)
	{
		return RemoveISR(irq, union_cast<Function>(method), (void*)user, args);
	}


	/// @brief Interrupt clear isr
	/// @param irq irq number
	/// @return Result::_OK / Result::_ERR
	void ClearISR(int irq)
	{
		List<Isr*>* isrs = &isrTabs[irq];

		for (Isr* isr = isrs->Begin(); !isrs->IsEnd(); isr = isrs->Next())
		{
			isrs->Remove(isr, isrs->GetNid());
		}
	}


	/// @brief Interrupt handler
	/// @param irq irq number
	void Handler(int irq)
	{
		List<Isr*>* isrs = &isrTabs[irq];
		
		if (isrs->IsEmpty())
		{
			if (++warnings[irq] >= warning_times)
			{
				debug->Error("IRQ %d no being handled correctly, system will halt on here", irq);
				while(1) {}
			}
			debug->Warn("IRQ %d has no interrupt service function", irq);
			return;
		}
		else
		{
			warnings[irq] = 0;
		}

		for (Isr* isr = isrs->Begin(); !isrs->IsEnd(); isr = isrs->Next())
		{
			(isr->func)(isr->user, isr->args);
		}
	}
};


///Register module
REGISTER_MODULE(ConcreteInterrupt, ModuleID::_interrupt, interrupt);
