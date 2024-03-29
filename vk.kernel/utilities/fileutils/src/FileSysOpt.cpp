//###########################################################################
// FileSysOpt.cpp
// Definitions of the functions that manage file
//
// $Copyright: Copyright (C) village
//###########################################################################
#include "FileSysOpt.h"
#include "Kernel.h"


/// @brief Constructor
FileSysOpt::FileSysOpt()
{
	filesys = (FileSys*)kernel->feature.GetModule("filesys");
	if (NULL == filesys)
	{
		kernel->debug.Error("file system feature not support");
		return;
	}
}


/// @brief Destructor
FileSysOpt::~FileSysOpt()
{
}


/// @brief File system move
/// @param from 
/// @param to 
/// @return 
bool FileSysOpt::Move(const char* from, const char* to)
{
	return false;
}


/// @brief File system copy
/// @param from 
/// @param to 
/// @return 
bool FileSysOpt::Copy(const char* from, const char* to)
{
	return false;
}


/// @brief File system remove
/// @param name 
/// @return 
bool FileSysOpt::Remove(const char* name)
{
	if (NULL != filesys)
	{
		volume = filesys->GetVolume(name);
	}
	
	if (NULL != volume)
	{
		volume->Remove(name);
	}

	return false;
}
