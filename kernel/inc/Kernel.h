//###########################################################################
// Kernel.h
// Declarations of the functions that manage module object
//
// $Copyright: Copyright (C) village
//###########################################################################
#ifndef __KERNEL_H__
#define __KERNEL_H__

#include "Module.h"

///Module create macro
#define CREATE_MODULE(mod, name) static struct _Mod_##name{_Mod_##name(){mod;}} const _mod_##name;

///Module register macro
#define REGISTER_MODULE(mod, id, name) CREATE_MODULE(Kernel::RegisterModule(mod, id), name)

///Kernel
class Kernel
{
private:
	//Structures
	struct ModuleNode
	{
		Module* module;
		ModuleNode* next;

		ModuleNode(Module* module = NULL) :
			module(module),
			next(NULL)
		{}
	};

	//Members
	static ModuleNode* list;
	static volatile bool isReady;
public:
	//Methods
	Kernel();
	static void Initialize();
	static void UpdateParams();
	static void Execute();
	static void FailSafe(int arg);
	static void RegisterModule(Module* module, uint32_t id);
	static void DeregisterModule(Module* module, uint32_t id);
};

#endif // !__KERNEL_H__
