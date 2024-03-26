//###########################################################################
// Loader.cpp
// Definitions of the functions that manage loader
//
// $Copyright: Copyright (C) village
//###########################################################################
#include "Loader.h"
#include "Kernel.h"
#include "rcParser.h"
#include "LibraryTool.h"
#include "ModuleTool.h"
#include "ElfExecutor.h"
#include "List.h"


/// @brief ConcreteLoader
class ConcreteLoader : public Loader
{
private:
	//Members
	List<ElfLoader*> libraries;
	List<ElfLoader*> modules;
	LibraryTool      libraryTool;
	ModuleTool       moduleTool;
	ElfExecutor      executor;
private:
	/// @brief Loader execute
	void Execute()
	{
		//Execute the first application of the village
		executor.Run(ElfExecutor::_Background, "/applications/taichi.exec");
	}
public:
	/// @brief Loader setup
	void Setup()
	{
		//Loading libraries
		Loading(_Load_Lib, "/libraries/_load_.rc");

		//Loading modules
		Loading(_Load_Mod, "/modules/_load_.rc");

		//Create task
		kernel->thread.CreateTask(this->GetName(), (Method)&ConcreteLoader::Execute, this);
	}


	/// @brief Exit
	void Exit()
	{
		libraries.Release();
		modules.Release();
	}


	/// @brief Loader load
	/// @param filename rc file path
	void Loading(int type, const char* filename)
	{
		RcParser* parser = new RcParser(filename);

		List<char*>& runcmds = parser->GetRunCmds();

		for (runcmds.End(); !runcmds.IsBegin(); runcmds.Prev())
		{
			if (_Load_Lib == type)
			{
				if (!libraryTool.Install(runcmds.Item())) break;
			}
			else if (_Load_Mod == type)
			{
				if (!moduleTool.Install(runcmds.Item())) break;
			}
		}

		parser->Release();
		delete parser;
	}


	/// @brief Get libraries
	/// @return libraries
	List<ElfLoader*>* GetLibraries()
	{
		return &libraries;
	}


	/// @brief Get modules
	/// @return modules
	List<ElfLoader*>* GetModules()
	{
		return &modules;
	}
};


///Register module
REGISTER_MODULE(new ConcreteLoader(), ModuleID::_serivce, loader);
