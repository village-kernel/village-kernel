//###########################################################################
// Loader.h
// Declarations of the functions that manage loader
//
// $Copyright: Copyright (C) village
//###########################################################################
#ifndef __LOADER_H__
#define __LOADER_H__

#include "Module.h"
#include "ElfExecutor.h"
#include "List.h"


/// @brief Loader
class Loader : public Module
{
public:
	//Enumerations
	enum LoadType
	{
		_Load_Lib = 0,
		_Load_Mod,
	};
public:
	//Methods
	virtual void Loading(int type, const char* filename) = 0;
	virtual List<ElfLoader*>* GetLibraries() = 0;
	virtual List<ElfLoader*>* GetModules() = 0;
};

#endif //!__LOADER_H__
