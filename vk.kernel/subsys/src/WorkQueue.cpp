//###########################################################################
// WorkQueue.cpp
// Definitions of the functions that manage work queue
//
// $Copyright: Copyright (C) village
//###########################################################################
#include "WorkQueue.h"
#include "Kernel.h"
#include "List.h"
#include "Cast.h"


/// @brief ConcreteWorkQueue
class ConcreteWorkQueue : public WorkQueue
{
private:
	//Members
	Thread*  thread;
	List<Work*> works;
private:
	/// @brief WorkQueue execute
	void Execute()
	{
		while (1)
		{
			for (Work* work = works.Begin(); !works.IsEnd(); work = works.Next())
			{
				if (_Waked == work->state)
				{
					work->state = _Running;
					if (work->ticks) thread->Sleep(work->ticks);
					(work->func)(work->user, work->args);
					work->state = _Finish;
				}
			}
		}
	}
public:
	/// @brief Constructor
	ConcreteWorkQueue()
		:thread(NULL)
	{
	}


	/// @brief WorkQueue setup
	void Setup()
	{
		//Gets the thread pointer
		thread = (Thread*)&kernel->thread;

		//Create work queue task
		thread->CreateTask(this->GetName(), (Method)&ConcreteWorkQueue::Execute, this);
	}


	/// @brief Exit
	void Exit()
	{
		works.Release();
	}


	/// @brief WorkQueue create
	/// @param func work function
	/// @param user work user
	/// @param args work args
	/// @param ticks delay ticks
	/// @return work
	Work* Create(Function func, void* user, void* args, uint32_t ticks)
	{
		Work* work = new Work(func, user, args, ticks);
		if (NULL != work) works.Add(work);
		return work;
	}


	/// @brief WorkQueue create
	/// @param method work method
	/// @param user work user
	/// @param args work args
	/// @param ticks delay ticks
	/// @return work
	Work* Create(Method method, Class* user, void* args, uint32_t ticks)
	{
		return Create(union_cast<Function>(method), (void*)user, args, ticks);
	}


	/// @brief WorkQueue delete
	/// @param work workqueue work
	/// @return result
	bool Delete(Work* work)
	{
		if (NULL != work && _Finish == work->state)
		{
			return works.Remove(work);
		}
		return false;
	}


	/// @brief WorkQueue schedule
	/// @param work workqueue work
	/// @return result
	bool Sched(Work* work)
	{
		if (NULL != work)
		{
			work->state = _Waked;
			return true;
		}
		return false;
	}
};


///Register module
REGISTER_MODULE(new ConcreteWorkQueue(), ModuleID::_feature, workQueue);
