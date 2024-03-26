//###########################################################################
// BaseExecutor.cpp
// Definitions of the functions that manage base executor
//
// $Copyright: Copyright (C) village
//###########################################################################
#include "Kernel.h"
#include "BaseExecutor.h"


/// @brief Constructor
BaseExecutor::BaseExecutor()
	:pid(0),
	argc(0),
	argv(NULL)
{
}


/// @brief Destructor
BaseExecutor::~BaseExecutor()
{
	regex.Clear();
}


/// @brief BaseExecutor Initialize
/// @param args run args
/// @return pid
int BaseExecutor::Run(Behavior behavior, const char* args)
{
	//Split args
	regex.Split(args);

	//Set argc and argv
	int    argc = regex.Size();
	char** argv = regex.ToArray();

	//Run with argc and argv
	return Run(behavior, argv[0], argc, argv);
}


/// @brief BaseExecutor Initialize
/// @param path file path
/// @param argc running argc
/// @param argv running argv
/// @return pid
int BaseExecutor::Run(Behavior behavior, const char* path, int argc, char* argv[])
{
	//Set argc and argv
	this->argc = argc;
	this->argv = argv;

	//Load, parser file and create task
	if ((pid = Execute(path)) == 0) return -1;

	//Wait for task done
	if (behavior == _Foreground) kernel->thread.WaitForTask(pid);

	return pid;
}


/// @brief BaseExecutor wait
/// @return result
bool BaseExecutor::Wait()
{
	return kernel->thread.WaitForTask(pid);
}
