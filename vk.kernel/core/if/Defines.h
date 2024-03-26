//###########################################################################
// Defines.h
// Declarations of the interface
//
// $Copyright: Copyright (C) village
//###########################################################################
#ifndef __DEFINES_H__
#define __DEFINES_H__

#include "stdint.h"
#include "stddef.h"
#include "stdlib.h"


/// @brief Driver id defines
enum DriverID
{
	_block,
	_network,
	_character,
	_miscellaneous,
	_dirverIdSize,
};


/// @brief Module id defines
enum ModuleID
{
	_feature,
	_serivce,
	_program,
	_moduleIdSize,
};


///Driver register macro
#define REGISTER_DRIVER(drv, id, name)                            \
static struct _Drv_##name {                                       \
	Driver* driver = drv;                                         \
	_Drv_##name() {                                               \
		driver->SetID(id);                                        \
		driver->SetName((char*)#name);                            \
		kernel->device.RegisterDriver(driver);                    \
	}                                                             \
	~_Drv_##name() {                                              \
		kernel->device.DeregisterDriver(driver);                  \
	}                                                             \
} const _drv_##name __attribute__((used,__section__(".drivers")))


///Module register macro
#define REGISTER_MODULE(mod, id, name)                            \
static struct _Mod_##name {                                       \
	Module* module = mod;                                         \
	_Mod_##name() {                                               \
		module->SetID(id);                                        \
		module->SetName((char*)#name);                            \
		kernel->feature.RegisterModule(module);                   \
	}                                                             \
	~_Mod_##name() {                                              \
		kernel->feature.DeregisterModule(module);                 \
	}                                                             \
} const _mod_##name __attribute__((used,__section__(".modules")))


///Environment marco
#ifdef KBUILD_NO_ENVIRONNEMNT
	///Export symbol marco
	#define EXPORT_SYMBOL_ALIAS(symbol, name)  /*export symbol alias*/

	///Export symbol marco
	#define EXPORT_SYMBOL(symbol)              /*export symbol*/
#else
	///Get method address
#if defined(ARCH_X86)
	#define marco_cast(src, addr)  __asm volatile("movl $"#src", %0" : "=r"(addr))
#elif defined(ARCH_ARM)
	#define marco_cast(src, addr)  __asm volatile("ldr %0, ="#src : "=r"(addr))
#endif
	///Export symbol marco
	#define EXPORT_SYMBOL_ALIAS(sym, name)                           \
	static struct _Sym_##name {                                      \
		_Sym_##name() {                                              \
			uint32_t symAddr = 0; marco_cast(sym, symAddr);          \
			kernel->symbol.Export(symAddr, #name);                   \
		}                                                            \
		~_Sym_##name() {                                             \
			kernel->symbol.Unexport(#name);                          \
		}                                                            \
	} const _sym_##name __attribute__((used,__section__(".symbols")))

	///Export symbol marco
	#define EXPORT_SYMBOL(symbol)          EXPORT_SYMBOL_ALIAS(symbol, symbol) 
#endif

#endif //!__DEFINES_H__
