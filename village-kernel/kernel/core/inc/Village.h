//###########################################################################
// Village.h
// Declarations of the functions that manage village
//
// $Copyright: Copyright (C) village
//###########################################################################
#ifndef __VILLAGE_H__
#define __VILLAGE_H__

#include "Kernel.h"
#include "System.h"
#include "Memory.h"
#include "Debug.h"
#include "Interrupt.h"
#include "Scheduler.h"
#include "Thread.h"
#include "WorkQueue.h"
#include "Symbol.h"
#include "Input.h"
#include "Device.h"
#include "Feature.h"
#include "FileSystem.h"
#include "Loader.h"


/// @brief Village
class Village : public Kernel
{
private:
	//Members
	ConcreteSystem      concreteSystem;
	ConcreteMemory      concreteMemory;
	ConcreteDebug       concreteDebug;
	ConcreteInterrupt   concreteInterrupt;
	ConcreteScheduler   concreteScheduler;
	ConcreteThread      concreteThread;
	ConcreteWorkQueue   concreteWorkQueue;
	ConcreteSymbol      concreteSymbol;
	ConcreteInput       concreteInput;
	ConcreteDevice      concreteDevice;
	ConcreteFeature     concreteFeature;
	ConcreteFileSystem  concreteFilesys;
	ConcreteLoader      concreteLoader;
private:
	//Methods
	Village();
	~Village();
public:
	//Methods
	void Setup();
	void Start();
	void Exit();
	const char* GetBuildDate();
	const char* GetBuildTime();
	const char* GetBuildVersion();
	const char* GetBuildGitCommit();

	//Static methods
	static Village& Instance();
};

#endif // !__VILLAGE_H__
