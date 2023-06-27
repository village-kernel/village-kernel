//###########################################################################
// Loader.h
// Declarations of the functions that manage loader
//
// $Copyright: Copyright (C) village
//###########################################################################
#ifndef __LOADER_H__
#define __LOADER_H__

#include "Templates.h"
#include "LibManager.h"
#include "ModManager.h"

///Loader
class Loader
{
private:
	//Enumerations
	enum LoadType
	{
		_Load_Lib = 0,
		_Load_Mod,
	};

	//Members
	LibManager libraries;
	ModManager modules;

	//Methods
	Loader();
	~Loader();
	void Loading(int type, const char* filename);
public:
	//Methods
	void Initialize();

	//Singleton Instance
	static Loader& Instance();
};

///Declarations loader reference
extern Loader& loader;

#endif //!__LOADER_H__
