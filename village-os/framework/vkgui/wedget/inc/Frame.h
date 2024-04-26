//###########################################################################
// Frame.h
// Declarations of the functions that manage stack frame
//
// $Copyright: Copyright (C) village
//###########################################################################
#ifndef __VK_FRAME_H__
#define __VK_FRAME_H__

#include "Wedget.h"

/// @brief Frame
class Frame : public Wedget
{
public:
	//Methods
	Frame();
	void Initialize();
	void Show();
};

#endif //!__VK_FRAME_H__
