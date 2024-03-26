//###########################################################################
// Scheduler.h
// Declarations of the functions that manage thread scheduler
//
// $Copyright: Copyright (C) village
//###########################################################################
#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include "Kernel.h"


/// @brief ConcreteScheduler
class ConcreteScheduler : public Scheduler, public Class
{
private:
	//Members
	bool isStartSchedule;

	//Methods
	void PendSVHandler();
	void SysTickHandler();
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
