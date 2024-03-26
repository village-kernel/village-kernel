//###########################################################################
// Thread.cpp
// Definitions of the functions that manage thread
//
// $Copyright: Copyright (C) village
//###########################################################################
#include "Thread.h"


/// @brief Constructor
ConcreteThread::ConcreteThread()
	:memory(NULL),
	system(NULL),
	scheduler(NULL)
{
}


/// @brief Destructor
ConcreteThread::~ConcreteThread()
{
}


/// @brief Thread setup
void ConcreteThread::Setup()
{
	//Gets the memory pointer
	memory = (Memory*)&kernel->memory;

	//Gets the system pointer
	system = (System*)&kernel->system;

	//Gets the scheduler pointer
	scheduler = (Scheduler*)&kernel->scheduler;

	//Frist task should be idle task and the pid is 0
	CreateTask("IdleTask", (Method)&ConcreteThread::IdleTask, this);

	//Set first node
	tasks.Begin();
}


/// @brief Exit
void ConcreteThread::Exit()
{
	tasks.Release();
}


/// @brief Thread task function handler 
/// @param function execute function
/// @param user execute user
void ConcreteThread::TaskHandler(Function function, void* user, void* args)
{
	if (NULL != function)
	{
		(function)(user, args);
	}
	TaskExit();
}


/// @brief Create new task
/// @param function task execute function
/// @param user task execute uesr
/// @param args task execute args
/// @return pid
int ConcreteThread::CreateTask(const char* name, Function function, void* user, void* args)
{
	//Create a new task and allocate stack space
	Task* task = new Task(memory->StackAlloc(task_stack_size), (char*)name);
	
	//Check whether stack allocation is successful
	if (NULL == task && 0 == task->stack) return -1;

	//Fill the stack content
	task->psp = task->stack - psp_frame_size;
	*(TaskContext*)task->psp = TaskContext
	(
		(uint32_t)union_cast<uint32_t>(&ConcreteThread::TaskHandler),
		(uint32_t)this,
		(uint32_t)function,
		(uint32_t)user,
		(uint32_t)args
	);

	//Add task into tasks list
	task->pid = tasks.Add(task);

	//return task pid
	return task->pid;
}


/// @brief Create new task for cpp
/// @param method task execute class method
/// @param user task execute class
/// @param args task execute args
/// @return pid
int ConcreteThread::CreateTask(const char* name, Method method, Class *user, void* args)
{
	return CreateTask(name, union_cast<Function>(method), (void*)user, args);
}


/// @brief Thread delete task
/// @param pid process id
/// @return result
int ConcreteThread::DeleteTask(int pid)
{
	Task* task = tasks.GetItem(pid);
	memory->Free(task->stack);
	return tasks.Remove(task, pid);
}


/// @brief Thread wait for task
/// @param pid process id
/// @return result
bool ConcreteThread::WaitForTask(int pid)
{
	//Gets the task
	Task* task = tasks.GetItem(pid);

	//Check task is valid
	if (NULL != task)
	{
		//Blocking wait
		while(task->state != TaskState::Exited) {}
		return true;
	}
	return false;
}


/// @brief Get tasks
/// @return 
List<ConcreteThread::Task*> ConcreteThread::GetTasks()
{
	return tasks;
}


/// @brief Thread sleep
/// @param ticks sleep ticks
void ConcreteThread::Sleep(uint32_t ticks)
{
	if(tasks.GetNid() > 0)
	{
		tasks.Item()->state = TaskState::Suspend;
		tasks.Item()->ticks = system->GetSysClkCounts() + ticks;
		scheduler->Sched(Scheduler::Unprivileged);
	}
}


/// @brief Thread Exit
void ConcreteThread::TaskExit()
{
	if(tasks.GetNid() > 0)
	{
		tasks.Item()->state = TaskState::Exited;
		DeleteTask(tasks.GetNid());
		scheduler->Sched(Scheduler::Unprivileged);
	}
}


/// @brief Save task PSP
/// @param psp process stack pointer
void ConcreteThread::SaveTaskPSP(uint32_t psp)
{
	tasks.Item()->psp = psp;
}


/// @brief Get current task psp
/// @return process stack pointer
uint32_t ConcreteThread::GetTaskPSP()
{
	return tasks.Item()->psp;
}


/// @brief Select next task, round-Robin scheduler
void ConcreteThread::SelectNextTask()
{
	while (1)
	{
		//Set next task as current task
		tasks.Next(); if (tasks.IsEnd()) tasks.Begin();

		//Check current task state
		if (TaskState::Suspend == tasks.Item()->state)
		{
			if(system->GetSysClkCounts() >= tasks.Item()->ticks)
			{
				tasks.Item()->state = TaskState::Running;
			}
		}

		//Break when task state is running
		if (TaskState::Running == tasks.Item()->state) break;
	}
}


/// @brief Idle task
#if defined(ARCH_X86)
void ConcreteThread::IdleTask() { while (1) { __asm volatile ("hlt" ::: "memory"); }}
#else
void ConcreteThread::IdleTask() { while (1) { __asm volatile("NOP"); } }
#endif
