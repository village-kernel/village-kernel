//###########################################################################
// Feature.cpp
// Definitions of the functions that manage module object
//
// $Copyright: Copyright (C) village
//###########################################################################
#include "Feature.h"


/// @brief Constructor
ConcreteFeature::ConcreteFeature()
	:isRuntime(false)
{
}


/// @brief Destructor
ConcreteFeature::~ConcreteFeature()
{
}


/// @brief Execute module object->Setup
void ConcreteFeature::Setup()
{
	isRuntime = false;
	
	for (int id = 0; id < _dirverIdSize; id++)
	{
		for (modules.Begin(); !modules.IsEnd(); modules.Next())
		{
			Module* module = modules.Item();

			if (module->GetID() == id) module->Setup();
		}
	}

	isRuntime = true;
}


/// @brief Execute module object->Exit
void ConcreteFeature::Exit()
{
	for (int id = _dirverIdSize - 1; id >= 0; id--)
	{
		for (modules.End(); !modules.IsBegin(); modules.Prev())
		{
			Module* module = modules.Item();

			if (module->GetID() == id) module->Exit();
		}
	}
}


/// @brief Register in runtime
/// @param module 
void ConcreteFeature::RegisterInRuntime(Module* module)
{
	if (isRuntime) module->Setup();
}


/// @brief Register module object
/// @param module module pointer
void ConcreteFeature::RegisterModule(Module* module)
{
	modules.Add(module, module->GetName());
	RegisterInRuntime(module);
}


/// @brief Deregister in runtime
/// @param module 
void ConcreteFeature::DeregisterInRuntime(Module* module)
{
	if (isRuntime) module->Exit();
}


/// @brief Deregister module object
/// @param module module pointer
void ConcreteFeature::DeregisterModule(Module* module)
{
	DeregisterInRuntime(module);
	modules.Remove(module);
}


/// @brief Get the module object by name
/// @param name module name
/// @return module
Module* ConcreteFeature::GetModule(const char* name)
{
	return modules.GetItemByName(name);
}
