//###########################################################################
// BinLoader.cpp
// Definitions of the functions that manage bin loader
//
// $Copyright: Copyright (C) village
//###########################################################################
#include "BinLoader.h"
#include "FileStream.h"
#include "Kernel.h"
#include "string.h"


/// @brief Constructor
/// @param filename 
BinLoader::BinLoader(const char* filename)
{
	if (NULL != filename) Load(filename);
}


/// @brief Deconstructor
BinLoader::~BinLoader()
{
	Exit();
}


/// @brief BinLoader load file
/// @param filename 
/// @return result
bool BinLoader::Load(const char* filename)
{
	//Save filename in local
	this->filename = new char[strlen(filename) + 1]();
	strcpy(this->filename, filename);

	//Load and mapping
	if (!LoadBin()) return false;

	//Output debug info
	kernel->debug.Output(Debug::_Lv2, "%s load done", filename);
	return true;
}


/// @brief BinLoader load bin file
/// @return 
bool BinLoader::LoadBin()
{
	FileStream file;

	if (file.Open(filename, FileMode::_Read))
	{
		int size = file.Size();
		bin.load = (uint32_t)new char[size]();

		if (bin.load && (file.Read((char*)bin.load, size) == size))
		{
			bin.exec = bin.load + *((uint32_t*)bin.load);
			kernel->debug.Output(Debug::_Lv1, "%s bin file load successful", filename);
			file.Close();
			return true;
		}

		file.Close();
	}

	kernel->debug.Error("%s bin file load failed", filename);
	return false;
}


/// @brief BinLoader execute
/// @param argc 
/// @param argv 
/// @return 
bool BinLoader::Execute(int argc, char* argv[])
{
	if (0 != bin.exec)
	{
		((Entry)bin.exec)(argc, argv);
		return true;
	}
	kernel->debug.Error("%s execute failed!", filename);
	return false;
}


/// @brief BinLoader exit
/// @return result
bool BinLoader::Exit()
{
	delete[] filename;
	delete[] (char*)bin.load;
	return true;
}
