//###########################################################################
// Kernel.h
// Declarations of the village interface
//
// $Copyright: Copyright (C) village
//###########################################################################
#ifndef __KERNEL_H__
#define __KERNEL_H__

#include "stdint.h"
#include "stddef.h"
#include "stdlib.h"
#include "List.h"
#include "Driver.h"
#include "Module.h"


/// @brief Debug
class Debug
{
public:
	//Debug level
	enum Level
	{
		_Lv0 = 0,
		_Lv1,
		_Lv2,
		_Lv3,
		_Lv4,
		_Lv5
	};
public:
	//Methods
	virtual void Log(const char* format, ...) = 0;
	virtual void Info(const char* format, ...) = 0;
	virtual void Error(const char* format, ...) = 0;
	virtual void Warn(const char* format, ...) = 0;
	virtual void Output(int level, const char* format, ...) = 0;
	virtual void SetDebugLevel(int level) = 0;
};


/// @brief Memory
class Memory
{
public:
	//Methods
	virtual uint32_t HeapAlloc(uint32_t size) = 0;
	virtual uint32_t StackAlloc(uint32_t size) = 0;
	virtual void Free(uint32_t memory, uint32_t size = 0) = 0;
	virtual uint32_t GetSize() = 0;
	virtual uint32_t GetUsed() = 0;
};


/// @brief Interrupt
class Interrupt
{
public:
	//Methods
	virtual int SetISR(int irq, Function func, void* user = NULL, void* args = NULL) = 0;
	virtual int SetISR(int irq, Method method, Class* user, void* args = NULL) = 0;
	virtual int AppendISR(int irq, Function func, void* user = NULL, void* args = NULL) = 0;
	virtual int AppendISR(int irq, Method method, Class* user, void* args = NULL) = 0;
	virtual bool RemoveISR(int irq, Function func, void* user = NULL, void* args = NULL) = 0;
	virtual bool RemoveISR(int irq, Method method, Class* user, void* args = NULL) = 0;
	virtual void ClearISR(int irq) = 0;
	virtual void Handler(int irq) = 0;
};


/// @brief Symbol
class Symbol
{
public:
	//Methods
	virtual void Export(uint32_t symAddr, const char* name) = 0;
	virtual void Unexport(const char* name) = 0;
	virtual uint32_t Search(const char* name) = 0;
};


/// @brief Device
class Device
{
public:
	//Methods
	virtual void RegisterDriver(Driver* driver) = 0;
	virtual void DeregisterDriver(Driver* driver) = 0;
	virtual Driver* GetDriver(const char* name) = 0;
};


/// @brief Feature
class Feature
{
public:
	//Methods
	virtual void RegisterModule(Module* module) = 0;
	virtual void DeregisterModule(Module* module) = 0;
	virtual Module* GetModule(const char* name) = 0;
};


/// @brief System
class System
{
public:
	virtual void SysTickCounter() = 0;
	virtual uint32_t GetSysClkCounts() = 0;
	virtual void DelayMs(uint32_t millis) = 0;
	virtual void EnableIRQ() = 0;
	virtual void DisableIRQ() = 0;
	virtual void Reboot() = 0;
};


/// @brief Thread
class Thread
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
		char*            name;
		uint32_t         pid;
		uint32_t         psp;
		uint32_t         ticks;
		uint32_t         stack;
		TaskState        state;
		
		Task(uint32_t stack = 0, char* name = NULL)
			:name(name),
			pid(-1),
			psp(0),
			ticks(0),
			stack(stack),
			state(TaskState::Suspend)
		{}
	};
public:
	///Methods
	virtual int CreateTask(const char* name, Function function, void* user = NULL, void* args = NULL) = 0;
	virtual int CreateTask(const char* name, Method method, Class *user, void* args = NULL) = 0;
	virtual int DeleteTask(int pid) = 0;
	virtual bool WaitForTask(int pid) = 0;
	virtual List<Task*> GetTasks() = 0;
	virtual void Sleep(uint32_t ticks) = 0;
	virtual void TaskExit() = 0;

	//Scheduler Methods
	virtual void SaveTaskPSP(uint32_t psp) = 0;
	virtual uint32_t GetTaskPSP() = 0;
	virtual void SelectNextTask() = 0;
	virtual void IdleTask() = 0;
};


/// @brief Scheduler
class Scheduler
{
public:
	//Enumerations
	enum Access
	{
		Unprivileged = 0,
		Privileged = 1,
	};
public:
	//Methods
	virtual void Start() = 0;
	virtual void Sched(Access access) = 0;
};


/// @brief Kernel
class Kernel
{
public:
	/// @brief Members
	System&      system;
	Memory&      memory;
	Debug&       debug;
	Interrupt&   interrupt;
	Scheduler&   scheduler;
	Thread&      thread;
	Symbol&      symbol;
	Device&      device;
	Feature&     feature;
public:
	/// @brief constructor
	Kernel(
		System&      system,
		Memory&      memory,
		Debug&       debug,
		Interrupt&   interrupt,
		Scheduler&   scheduler,
		Thread&      thread,
		Symbol&      symbol,
		Device&      device,
		Feature&     feature
	)
		:system(system),
		memory(memory),
		debug(debug),
		interrupt(interrupt),
		scheduler(scheduler),
		thread(thread),
		symbol(symbol),
		device(device),
		feature(feature)
	{}

	/// @brief Destructor
	virtual ~Kernel() {}

	/// @brief Kernel Methods
	virtual void Setup() = 0;
	virtual void Start() = 0;
	virtual void Exit() = 0;

	/// @brief Kernel build info 
	virtual const char* GetBuildDate() = 0;
	virtual const char* GetBuildTime() = 0;
	virtual const char* GetBuildVersion() = 0;
	virtual const char* GetBuildGitCommit() = 0;
};

/// @brief Declarations kernel pointer
extern Kernel* kernel;

#endif //!__KERNEL_H__
