//###########################################################################
// FileSystem.h
// Declarations of the functions that manage file system
//
// $Copyright: Copyright (C) village
//###########################################################################
#ifndef __FILE_SYSTEM_H__
#define __FILE_SYSTEM_H__

#include "Module.h"
#include "FileSys.h"
#include "FileOpts.h"


/// @brief FileSystem
class FileSystem : public Module
{
public:
	//Methods
	virtual void RegisterFS(FileSys* fs, const char* name) = 0;
	virtual void DeregisterFS(FileSys* fs, const char* name) = 0;
	virtual int RegisterOpts(FileOpts* opts) = 0;
	virtual int DeregisterOpts(FileOpts* opts) = 0;
	virtual FileOpts* GetFileOpts(const char* name) = 0;
};

#endif //!__FILE_SYSTEM_H__
