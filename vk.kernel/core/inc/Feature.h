//###########################################################################
// Feature.h
// Declarations of the functions that manage module object
//
// $Copyright: Copyright (C) village
//###########################################################################
#ifndef __FEATURE_H__
#define __FEATURE_H__

#include "Kernel.h"
#include "Module.h"
#include "List.h"


/// @brief ConcreteFeature
class ConcreteFeature : public Feature
{
private:
	//Members
	bool isRuntime;
	List<Module*> modules;

	//Methods
	void RegisterInRuntime(Module* module);
	void DeregisterInRuntime(Module* module);
public:
	//Methods
	ConcreteFeature();
	~ConcreteFeature();
	void Setup();
	void Exit();
	void RegisterModule(Module* module);
	void DeregisterModule(Module* module);
	Module* GetModule(const char* name);
};

#endif // !__FEATURE_H__
