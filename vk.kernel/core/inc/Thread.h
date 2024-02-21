//###########################################################################
// Thread.h
// Declarations of the functions that manage thread
//
// $Copyright: Copyright (C) village
//###########################################################################
#ifndef __THREAD_H__
#define __THREAD_H__

#include "Module.h"
#include "Registers.h"
#include "Templates.h"

#ifndef TASK_STACK
#define TASK_STACK      1024
#endif

/// @brief Thread
class Thread : public Module
{
public:
	//Enumerations
	enum TaskState 
	{
		Running = 0,
		Suspend,
		Blocked,
		Exited,
	};

	//Structures
	struct Task 
	{
		TaskState        state;
		uint32_t         stack;
		uint32_t         ticks;
		uint32_t         psp;

		Task(uint32_t stack = 0)
			:state(TaskState::Suspend),
			stack(stack),
			ticks(0),
			psp(0)
		{}
	};
public:
	///Methods
	virtual int CreateTask(const char* name, Function function, void* user = NULL, void* args = NULL) = 0;
	virtual int CreateTask(const char* name, Method method, Class *user, void* args = NULL) = 0;
	virtual int DeleteTask(int pid) = 0;
	virtual int WaitForTask(int pid) = 0;
	virtual List<Task*> GetTasks() = 0;
	virtual void Sleep(uint32_t ticks) = 0;
	virtual void TaskExit() = 0;

	//Scheduler Methods
	virtual void SaveTaskPSP(uint32_t psp) = 0;
	virtual uint32_t GetTaskPSP() = 0;
	virtual void SelectNextTask() = 0;
	virtual void IdleTask() = 0;
};

#endif // !__THREAD_H__
