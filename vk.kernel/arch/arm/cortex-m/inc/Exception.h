//###########################################################################
// Exception.h
// Declarations of the functions that manage exception
//
// $Copyright: Copyright (C) village
//###########################################################################
#ifndef __EXCEPTION_H__
#define __EXCEPTION_H__

#include "Kernel.h"

/// @brief Exception
class Exception 
{
public:
	//Methods
	Exception();
	~Exception();
	void Setup();
	void Exit();
};

#endif //!__EXCEPTION_H__
