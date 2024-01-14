//###########################################################################
// Window.cpp
// Definitions of the functions that manage window
//
// $Copyright: Copyright (C) village
//###########################################################################
#include "Window.h"
#include "Display.h"


/// @brief 
void Window::Initialize()
{

}


/// @brief Initialize
void Window::Show()
{
	int locX = GetLocX();
	int locY = GetLocY();
	display->DrawRectangle(locX, locY, width - 1, height - 1);
	Wedget::Show();
}
