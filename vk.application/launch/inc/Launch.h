//###########################################################################
// Launch.h
// The overall framework of the launch
//
// $Copyright: Copyright (C) village
//###########################################################################
#ifndef __LAUNCH_H__
#define __LAUNCH_H__

#include "Graphics.h"
#include "Village.h"
#include "Input.h"


///Lanuch
class Launch : public Class
{
private:
	//Members
	Graphics graphics;
	Window*  mainwin;
	Village  village;

	//Methods
	void UpdateInput(Input::InputMove* input);
public:
	//Methods
	Launch();
	~Launch();
	void Initialize(const char* drvname);
	void Execute();
};

#endif //!__LAUNCH_H__
