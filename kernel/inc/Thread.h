//###########################################################################
// Thread.h
// Declarations of the functions that manage thread
//
// $Copyright: Copyright (C) village
//###########################################################################
#ifndef __THREAD_H__
#define __THREAD_H__

#include "Defines.h"
#include "Registers.h"
#include "Templates.h"

///Thread
class Thread : public Class
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
private:
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

	//Static constants
	static const uint32_t task_stack_size = 1024;
	static const uint32_t psp_frame_size = sizeof(Registers) >> 2;
	
	//Members
	List<Task> tasks;

	//Methods
	Thread();
	~Thread();
	static void IdleTask();
	static void FuncHandler(Function function, char* argv = NULL);
	static void MethodHandler(Class *user, Method method, char* argv = NULL);
public:
	///Methods
	void Initialize();
	void Execute();
	int CreateTask(Function function, char* argv = NULL);
	int CreateTaskCpp(Class *user, Method method, char* argv = NULL);
	int DeleteTask(int pid);
	int WaitForTask(int pid);
	void Sleep(uint32_t ticks);
	void Exit();

	//Scheduler Methods
	void SaveTaskPSP(uint32_t psp);
	uint32_t GetTaskPSP();
	void SelectNextTask();

	//Singleton Instance
	static Thread& Instance();
};

///Declarations thread reference
extern Thread& thread;

#endif // !__THREAD_H__
