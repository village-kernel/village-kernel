//###########################################################################
// WorkQueue.h
// Declarations of the functions that manage work queue
//
// $Copyright: Copyright (C) village
//###########################################################################
#ifndef __WORK_QUEUE_H__
#define __WORK_QUEUE_H__

#include "WorkQueue.h"
#include "Kernel.h"
#include "List.h"
#include "Cast.h"


/// @brief ConcreteWorkQueue
class ConcreteWorkQueue : public WorkQueue, public Class
{
private:
	//Members
	Thread*     thread;
	List<Work*> works;

	//Methods
	void Execute();
public:
	//Methods
	ConcreteWorkQueue();
	~ConcreteWorkQueue();
	void Setup();
	void Exit();
	Work* Create(Function func, void* user = NULL, void* args = NULL, uint32_t ticks = 0);
	Work* Create(Method method, Class* user, void* args = NULL, uint32_t ticks = 0);
	bool Delete(Work* work);
	bool Sched(Work* work);
};

#endif // !__WORK_QUEUE_H__
