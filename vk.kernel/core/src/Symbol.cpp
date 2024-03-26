//###########################################################################
// Symbol.cpp
// Definitions of the functions that manage symbol object
//
// $Copyright: Copyright (C) village
//###########################################################################
#include "Symbol.h"
#include "string.h"


/// @brief Constructor
ConcreteSymbol::ConcreteSymbol()
{
}


/// @brief Destructor
ConcreteSymbol::~ConcreteSymbol()
{
}


/// @brief Setup
void ConcreteSymbol::Setup()
{
	
}


/// @brief Exit
void ConcreteSymbol::Exit()
{
	entrys.Release();
}


/// @brief Export symbol
/// @param symAddr symbol address
/// @param name symbol name
void ConcreteSymbol::Export(uint32_t symAddr, const char* name)
{
	entrys.Add(new Entry(name, symAddr));
}


/// @brief Unexport symbol
/// @param name symbol name
void ConcreteSymbol::Unexport(const char* name)
{
	for (entrys.Begin(); !entrys.IsEnd(); entrys.Next())
	{
		Entry* entry = entrys.Item();

		if (0 == strcmp(name, entry->name))
		{
			entrys.Remove(entry, entrys.GetNid()); break;
		}
	}
}


/// @brief Search symbol by name and return addr
/// @param name symbol name
/// @return symbol address
uint32_t ConcreteSymbol::Search(const char* name)
{
	for (entrys.Begin(); !entrys.IsEnd(); entrys.Next())
	{
		Entry* entry = entrys.Item();

		if (0 == strcmp(name, entry->name))
		{
			return entry->addr;
		}
	}
	return 0;
}
