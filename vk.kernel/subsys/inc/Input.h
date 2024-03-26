//###########################################################################
// Input.h
// Declarations of the functions that manage input
//
// $Copyright: Copyright (C) village
//###########################################################################
#ifndef __INPUT_H__
#define __INPUT_H__

#include "Module.h"


/// @brief Input
class Input : public Module
{
public:
	//Input type
	enum Type
	{
		_Event = 0,
		_Movement,
		_AllType,
	};

	//Input event
	struct Event
	{
		int keycode;
		int status;

		Event()
			:keycode(0),
			status(0)
		{}
	};

	//Input movement
	struct Movement
	{
		int axisX;
		int axisY;
		int axisZ;

		Movement()
			:axisX(0),
			axisY(0),
			axisZ(0)
		{}
	};
public:
	//Methods
	virtual void Attach(Type type, Method method, Class* user) = 0;
	virtual void Attach(Type type, Function func, void* user = NULL) = 0;
	virtual void Detach(Type type, Method method, Class* user) = 0;
	virtual void Detach(Type type, Function func, void* user = NULL) = 0;
	virtual void ReportEvent(int keycode, int status) = 0;
	virtual void ReportMovement(int axisX, int axisY, int axisZ) = 0;
};

#endif //!__INPUT_H__
