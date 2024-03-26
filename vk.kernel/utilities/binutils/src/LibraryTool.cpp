//###########################################################################
// LibraryTool.cpp
// Definitions of the functions that manage library
//
// $Copyright: Copyright (C) village
//###########################################################################
#include "LibraryTool.h"
#include "Kernel.h"
#include "Loader.h"


/// @brief Constructor
LibraryTool::LibraryTool()
{
}


/// @brief Destructor
LibraryTool::~LibraryTool()
{
}


/// @brief Library tool Get libraries
List<ElfLoader*>* LibraryTool::GetLibraries()
{
	//Get the loader module
	Loader* loader = (Loader*)kernel->feature.GetModule("loader");
	if (NULL == loader)
	{
		kernel->debug.Error("loader feature not support");
		return NULL;
	}

	//Get the libraries pointer
	List<ElfLoader*>* libraries = loader->GetLibraries();
	if (NULL == libraries)
	{
		kernel->debug.Error("get libraries address failed");
		return NULL;
	}

	return libraries;
}


/// @brief library installer
/// @param library 
/// @return result
bool LibraryTool::Install(const char* filename)
{
	bool isInstalled = false;

	//Check filename is valid
	if (NULL == filename)
	{
		kernel->debug.Error("%s library not a valid name", filename);
		return false;
	}

	//Get the libraries pointer
	List<ElfLoader*>* libraries = GetLibraries();
	if (NULL == libraries) return false;

	//Check the library if it has been installed
	for (ElfLoader* library = libraries->Begin(); !libraries->IsEnd(); library = libraries->Next())
	{
		if (0 == strcmp(filename, library->GetFileName()))
		{
			isInstalled = true;
			kernel->debug.Output(Debug::_Lv2, "%s library has already been installed", filename);
			break;
		}
	}

	//Install library if it has not install
	if (false == isInstalled)
	{
		ElfLoader* library = new ElfLoader();

		library->IgnoreUnresolvedSymbols();

		if (library->Load(filename))
		{
			library->FillBssZero();
			library->InitArray();
			libraries->Add(library);
			kernel->debug.Output(Debug::_Lv2, "%s library install successful", filename);
		}
		else
		{
			kernel->debug.Error("%s library install failed", filename);
			return false;
		}
	}

	return true;
}


/// @brief library uninstaller
/// @param library 
/// @return 
bool LibraryTool::Uninstall(const char* filename)
{
	//Check filename is valid
	if (NULL == filename)
	{
		kernel->debug.Error("%s library not a valid name", filename);
		return false;
	}

	//Get the libraries pointer
	List<ElfLoader*>* libraries = GetLibraries();
	if (NULL == libraries) return false;

	//Search library and remove it
	for (ElfLoader* library = libraries->Begin(); !libraries->IsEnd(); library = libraries->Next())
	{
		if (0 == strcmp(filename, library->GetFileName()))
		{
			library->FiniArray();
			libraries->Remove(library);
			delete library;
			kernel->debug.Output(Debug::_Lv2, "%s library uninstall successful", filename);
			return true;	
		}
	}

	kernel->debug.Error("%s library not found", filename);
	return false;
}


/// @brief Search symbol
/// @param symbol 
/// @return symbol address
uint32_t LibraryTool::SearchSymbol(const char* symbol)
{
	//Check symbol is valid
	if (NULL == symbol)
	{
		kernel->debug.Error("%s symbol not a valid name", symbol);
		return 0;
	}

	//Get the libraries pointer
	List<ElfLoader*>* libraries = GetLibraries();
	if (NULL == libraries) return false;

	//Search symbol
	for (ElfLoader* lib = libraries->Begin(); !libraries->IsEnd(); lib = libraries->Next())
	{
		uint32_t symAddr = lib->GetDynSymAddrByName(symbol);
		if (0 != symAddr) return symAddr;
	}

	return 0;
}
