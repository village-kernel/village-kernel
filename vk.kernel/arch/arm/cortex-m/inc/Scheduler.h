//###########################################################################
// Scheduler.h
// Declarations of the functions that manage thread scheduler
//
// $Copyright: Copyright (C) village
//###########################################################################
#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include "Kernel.h"


/// @brief Scheduler
class ConcreteScheduler : public Scheduler
{
private:
	//Members
	bool isStartSchedule;
public:
	//Methods
	ConcreteScheduler();
	~ConcreteScheduler();
	void Setup();
	void Exit();
	void Start();
	void Sched(Access access);
};

#endif // !__SCHEDULER_H__
