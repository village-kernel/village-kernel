//###########################################################################
// operator.cpp
// Definitions of the functions that memory
//
// $Copyright: Copyright (C) village
//###########################################################################
#include "stdint.h"
#include "stddef.h"
#include "stdlib.h"


/// @brief Override new
/// @param size byte size
/// @return address
void *operator new(size_t size)
{
	return NULL;
}


/// @brief Override new[]
/// @param size byte size
/// @return address
void *operator new[](size_t size)
{
	return NULL;
}


/// @brief Override delete
/// @param ptr address
void operator delete(void *ptr)
{

}


/// @brief Override delete[]
/// @param ptr address
void operator delete[](void *ptr)
{

}


/// @brief Override delete
/// @param ptr address
/// @param size byte size
void operator delete(void *ptr, size_t size)
{

}


/// @brief Override delete[]
/// @param ptr address
/// @param size byte size
void operator delete[](void *ptr, size_t size)
{

}
