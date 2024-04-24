//###########################################################################
// Input.cpp
// The overall framework of the input
//
// $Copyright: Copyright (C) village
//###########################################################################
#include "Input.h"


/// @brief Constructor
Input::Input()
{
} 


/// @brief Destructor
Input::~Input()
{
}


/// @brief Initialize
void Input::Initialize(const char* keyboard, const char* mouse)
{
	this->keyboard.Initialize(keyboard);
	this->mouse.Initialize(mouse);
}


/// @brief Execute
void Input::Execute()
{
	keyboard.Execute();
	mouse.Execute();
	while(1) {}
}


/// @brief main
int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		return -1;
	}
	else
	{
		Input input;
		input.Initialize(argv[1], argv[2]);
		input.Execute();
		return 0;
	}
}