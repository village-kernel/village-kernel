//###########################################################################
// Thread.h
// Declarations of the functions that manage thread
//
// $Copyright: Copyright (C) village
//###########################################################################
#ifndef __THREAD_H__
#define __THREAD_H__

#include "Kernel.h"
#include "Registers.h"
#include "Cast.h"
#include "List.h"


#ifndef TASK_STACK
#define TASK_STACK      1024
#endif


/// @brief ConcreteThread
class ConcreteThread : public Thread, public Class
{
private:
	//Static constants
	static const uint32_t task_stack_size = TASK_STACK;
	static const uint32_t psp_frame_size = sizeof(TaskContext) >> 2;
	
	//Members
	List<Task*> tasks;
	Memory*     memory;
	System*     system;
	Scheduler*  scheduler;

	//Members
	void TaskHandler(Function function, void* user, void* args);
public:
	///Methods
	ConcreteThread();
	~ConcreteThread();
	void Setup();
	void Exit();

	///Thread Methods
	int CreateTask(const char* name, Function function, void* user = NULL, void* args = NULL);
	int CreateTask(const char* name, Method method, Class *user, void* args = NULL);
	int DeleteTask(int pid);
	bool WaitForTask(int pid);
	List<Task*> GetTasks();
	void Sleep(uint32_t ticks);
	void TaskExit();

	//Scheduler Methods
	void SaveTaskPSP(uint32_t psp);
	uint32_t GetTaskPSP();
	void SelectNextTask();
	void IdleTask();
};

#endif // !__THREAD_H__
