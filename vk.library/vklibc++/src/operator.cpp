//###########################################################################
// operator.cpp
// Definitions of the functions that memory
//
// $Copyright: Copyright (C) village
//###########################################################################
#include "stdint.h"
#include "stddef.h"
#include "stdlib.h"
#include "kernel.h"


/// @brief Override new
/// @param size byte size
/// @return address
void *operator new(size_t size)
{
	return (void*)kernel->memory.HeapAlloc((uint32_t)size);
}


/// @brief Override new[]
/// @param size byte size
/// @return address
void *operator new[](size_t size)
{
	return (void*)kernel->memory.HeapAlloc((uint32_t)size);
}


/// @brief Override delete
/// @param ptr address
void operator delete(void *ptr)
{
	kernel->memory.Free((uint32_t)ptr);
}


/// @brief Override delete[]
/// @param ptr address
void operator delete[](void *ptr)
{
	kernel->memory.Free((uint32_t)ptr);
}


/// @brief Override delete
/// @param ptr address
/// @param size byte size
void operator delete(void *ptr, size_t size)
{
	kernel->memory.Free((uint32_t)ptr, size);
}


/// @brief Override delete[]
/// @param ptr address
/// @param size byte size
void operator delete[](void *ptr, size_t size)
{
	kernel->memory.Free((uint32_t)ptr, size);
}


/// @brief malloc
/// @param size 
/// @return 
extern "C" void* malloc(size_t size)
{
	return (void*)kernel->memory.HeapAlloc((uint32_t)size);
}


/// @brief free
/// @param ptr 
extern "C" void free(void* ptr)
{
	kernel->memory.Free((uint32_t)ptr);
}


/// @brief rand
/// @param  
/// @return 
extern "C" int rand(void)
{
	return kernel->system.GetSysClkCounts();
}
