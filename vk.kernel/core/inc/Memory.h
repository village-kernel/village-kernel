//###########################################################################
// Memory.h
// Declarations of the functions that manage memory
//
// $Copyright: Copyright (C) village
//###########################################################################
#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "Kernel.h"


/// @brief ConcreteMemory
class ConcreteMemory : public Memory
{
private:
	//Structures
	struct Map 
	{
		uint32_t addr;
		uint32_t size;

		Map(uint32_t addr = 0, uint32_t size = 0):
			addr(addr),
			size(size)
		{}
	};

	struct MapNode
	{
		Map map;
		MapNode* prev;
		MapNode* next;

		MapNode(Map map = 0):
			map(map),
			prev(NULL),
			next(NULL)
		{}
	};
private:
	//Static constants
	const static uint8_t  align = 4;
	const static uint32_t size_of_node = sizeof(MapNode);
	const static uint32_t kernel_rsvd_heap  = 0x4000;  //10k
	const static uint32_t kernel_rsvd_stack = 0x3200;  //8k

	//Sram parameters
	bool isMemReady;
	uint32_t sram_start;
	uint32_t sram_ended;
	uint32_t sram_used;
	uint32_t sbrk_heap;

	//Members
	MapNode* head;
	MapNode* tail;
	MapNode* curr;
public:
	//Methods
	ConcreteMemory();
	~ConcreteMemory();
	void Setup();
	void Exit();
	uint32_t HeapAlloc(uint32_t size);
	uint32_t StackAlloc(uint32_t size);
	void Free(uint32_t memory, uint32_t size = 0);
	uint32_t GetSize();
	uint32_t GetUsed();
};

#endif //!__MEMORY_H__
