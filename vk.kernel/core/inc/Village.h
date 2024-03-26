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
#include "Debug.h"
#include "Memory.h"
#include "Symbol.h"
#include "Device.h"
#include "Feature.h"
#include "Thread.h"
#include "Scheduler.h"
#include "Interrupt.h"


/// @brief Village
class Village : public Kernel
{
private:
	//Members
	ConcreteSystem    concreteSystem;
	ConcreteDebug     concreteDebug;
	ConcreteMemory    concreteMemory;
	ConcreteSymbol    concreteSymbol;
	ConcreteDevice    concreteDevice;
	ConcreteFeature   concreteFeature;
	ConcreteThread    concreteThread;
	ConcreteScheduler concreteScheduler;
	ConcreteInterrupt concreteInterrupt;
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
