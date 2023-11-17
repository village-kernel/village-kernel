//###########################################################################
// FileSysOpt.cpp
// Definitions of the functions that manage file
//
// $Copyright: Copyright (C) village
//###########################################################################
#include "FileSysOpt.h"
#include "FileSystem.h"


/// @brief Constructor
FileSysOpt::FileSysOpt()
{
}


/// @brief Destructor
FileSysOpt::~FileSysOpt()
{
}


/// @brief File system move
/// @param from 
/// @param to 
/// @return 
int FileSysOpt::Move(const char* from, const char* to)
{
	return _ERR;
}


/// @brief File system copy
/// @param from 
/// @param to 
/// @return 
int FileSysOpt::Copy(const char* from, const char* to)
{
	return _ERR;
}


/// @brief File system remove
/// @param name 
/// @return 
int FileSysOpt::Remove(const char* name)
{
	return _ERR;
}


/// @brief File system move
/// @param from 
/// @param to 
/// @return 
int FileSysOpt::MoveDir(const char* from, const char* to)
{
	return _ERR;
}


/// @brief File system copy
/// @param from 
/// @param to 
/// @return 
int FileSysOpt::CopyDir(const char* from, const char* to)
{
	return _ERR;
}


/// @brief File system remove
/// @param name 
/// @return 
int FileSysOpt::RemoveDir(const char* name)
{
	return _ERR;
}
