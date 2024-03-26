//###########################################################################
// Input.cpp
// Definitions of the functions that manage input
//
// $Copyright: Copyright (C) village
//###########################################################################
#include "Input.h"
#include "Kernel.h"
#include "List.h"
#include "Cast.h"


/// @brief ConcreteInput
class ConcreteInput : public Input
{
private:
	//Structures
	struct Observer
	{
		Function    func;
		void*       user;

		Observer(Function func = NULL, void* user = NULL)
			:func(func),
			user(user)
		{}
	};

	//Members
	List<Observer*> observers[_AllType];
	Event           event;
	Movement        movement;
private:
	/// @brief Input base attach
	/// @param observer 
	/// @param func 
	/// @param user 
	void Attach(List<Observer*>& observers, Function func, void* user)
	{
		observers.Add(new Observer(func, user));
	}


	/// @brief Input base detach
	/// @param observer 
	/// @param func 
	/// @param user 
	void Detach(List<Observer*>& observers, Function func, void* user)
	{
		for (observers.Begin(); !observers.IsEnd(); observers.Next())
		{
			Observer* observer = observers.Item();

			if ((func == observer->func) &&
				(user == observer->user))
			{
				observers.Remove(observer, observers.GetNid());
			}
		}
	}


	/// @brief Input base notify
	/// @param observer 
	/// @param argv 
	void Notify(List<Observer*>& observers, void* argv)
	{
		for (observers.Begin(); !observers.IsEnd(); observers.Next())
		{
			Observer* observer = observers.Item();
			(observer->func)(observer->user, argv);
		}
	}
public:
	/// @brief Setup
	void Setup()
	{

	}


	/// @brief Exit
	void Exit()
	{

	}


	/// @brief Input attach
	/// @param method 
	/// @param user 
	void Attach(Type type, Method method, Class* user)
	{
		Attach(observers[type], union_cast<Function>(method), (void*)user);
	}


	/// @brief Input Attach
	/// @param func 
	/// @param user 
	void Attach(Type type, Function func, void* user)
	{
		Attach(observers[type], func, user);
	}


	/// @brief Input Detach
	/// @param method 
	/// @param user 
	void Detach(Type type, Method method, Class* user)
	{
		Detach(observers[type], union_cast<Function>(method), (void*)user);
	}


	/// @brief Input Detach
	/// @param func 
	/// @param user 
	void Detach(Type type, Function func, void* user)
	{
		Detach(observers[type], func, user);
	}


	/// @brief Input report event
	/// @param keycode 
	/// @param status 
	void ReportEvent(int keycode, int status)
	{
		event.keycode = keycode;
		event.status  = status;
		Notify(observers[_Event], (void*)&event);
	}


	/// @brief Input report movement
	/// @param axisX 
	/// @param axisY 
	/// @param axisZ 
	void ReportMovement(int axisX, int axisY, int axisZ)
	{
		movement.axisX = axisX;
		movement.axisY = axisY;
		movement.axisZ = axisZ;
		Notify(observers[_Movement], (void*)&movement);
	}
};


///Register module
REGISTER_MODULE(new ConcreteInput(), ModuleID::_feature, input);
