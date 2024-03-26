//###########################################################################
// Symbol.h
// Declarations of the functions that manage symbol object
//
// $Copyright: Copyright (C) village
//###########################################################################
#ifndef __SYMBOL_H__
#define __SYMBOL_H__

#include "Kernel.h"
#include "List.h"


/// @brief ConcreteSymbol
class ConcreteSymbol : public Symbol
{
private:
	//Structures
	struct Entry
	{
		const char* name;
		uint32_t    addr;

		Entry(const char* name, uint32_t addr):
			name(name),
			addr(addr)
		{}
	};

	//Members
	List<Entry*> entrys;
public:
	//Methods
	ConcreteSymbol();
	~ConcreteSymbol();
	void Setup();
	void Exit();
	void Export(uint32_t symAddr, const char* name);
	void Unexport(const char* name);
	uint32_t Search(const char* name);
};

#endif //!__SYMBOL_H__
