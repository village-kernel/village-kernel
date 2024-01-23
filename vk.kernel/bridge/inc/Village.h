//###########################################################################
// Village.h
// Declarations of the functions that manage village
//
// $Copyright: Copyright (C) village
//###########################################################################
#ifndef __VILLAGE_H__
#define __VILLAGE_H__

#include "Kernel.h"
#include "Input.h"


/// @brief Village
class Village
{
public:
	//Methods
	Village();
	~Village();

	void Reboot();
	void Sleep(uint32_t ticks);
	List<Thread::Task*> GetTasks();
	
	uint32_t GetMemSize();
	uint32_t GetMemUsed();

	Driver* GetDriver(uint32_t id);
	Driver* GetDriver(const char* name);

	Module* GetModule(uint32_t id);
	Module* GetModule(const char* name);

	void AttachInput(Input::Type type, Method method, Class *user);
	void DetachInput(Input::Type type, Method method, Class *user);

	void SetDebugLevel(int level);

	const char* GetBuildDate();
	const char* GetBuildTime();
	const char* GetBuildVersion();
	const char* GetBuildGitCommit();
};

#endif //!__VILLAGE_H__
