//###########################################################################
// LibManager.h
// Declarations of the functions that manage library
//
// $Copyright: Copyright (C) village
//###########################################################################
#ifndef __LIB_MANAGER_H__
#define __LIB_MANAGER_H__

#include "Defines.h"
#include "Templates.h"
#include "ElfLoader.h"

/// @brief LibManager
class LibManager
{
private:
	//Members
	static List<ElfLoader> libraries;
public:
	//Methods
	LibManager();
	~LibManager();
	bool Install(const char* filename);
	bool Uninstall(const char* filename);
	uint32_t SearchSymbol(const char* symbol);

	/// @brief Get libraries
	/// @return libraries
	List<ElfLoader>* GetLibraries() { return &libraries; }
};

#endif //!__LIB_MANAGER_H__
