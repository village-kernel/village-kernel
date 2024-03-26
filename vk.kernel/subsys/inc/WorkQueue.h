//###########################################################################
// WorkQueue.h
// Declarations of the functions that manage work queue
//
// $Copyright: Copyright (C) village
//###########################################################################
#ifndef __WORK_QUEUE_H__
#define __WORK_QUEUE_H__

#include "Module.h"


/// @brief WorkQueue
class WorkQueue : public Module
{
public:
	//Enumerations
	enum State
	{
		_Suspend = 0,
		_Waked,
		_Running,
		_Finish,
	};

	//Structures
	struct Work
	{
		Function func;
		void*    user;
		void*    args;
		uint32_t ticks;
		State    state;

		Work(Function func, void* user, void* args, uint32_t ticks)
			:func(func),
			user(user),
			args(args),
			ticks(ticks),
			state(_Suspend)
		{}
	};
public:
	//Methods
	virtual Work* Create(Function func, void* user = NULL, void* args = NULL, uint32_t ticks = 0) = 0;
	virtual Work* Create(Method method, Class* user, void* args = NULL, uint32_t ticks = 0) = 0;
	virtual bool Delete(Work* work) = 0;
	virtual bool Sched(Work* work) = 0;
};

#endif // !__WORK_QUEUE_H__
