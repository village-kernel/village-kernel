//###########################################################################
// ElfExecutor.cpp
// Definitions of the functions that manage elf executor
//
// $Copyright: Copyright (C) village
//###########################################################################
#include "Kernel.h"
#include "ElfExecutor.h"


/// @brief ElfExecutor Execute
/// @param path 
/// @return pid
int ElfExecutor::Execute(const char* path)
{
	//Load, parser and execute bin file
	if (!elf.Load(path)) return 0;

	//Create a sandboxed thread to run the app
	return kernel->thread.CreateTask(path, (Method)&ElfExecutor::Sandbox, this);
}


/// @brief ElfExecutor execute app
void ElfExecutor::Sandbox()
{
	elf.Execute(NULL, argc, argv);
	elf.Exit();
}
