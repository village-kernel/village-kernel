//###########################################################################
// Device.h
// Declarations of the functions that manage device
//
// $Copyright: Copyright (C) village
//###########################################################################
#ifndef __DEVICE_H__
#define __DEVICE_H__

#include "Kernel.h"
#include "Driver.h"
#include "List.h"


/// @brief ConcreteDevice
class ConcreteDevice : public Device
{
private:
	//Members
	List<Driver*> drivers;
public:
	//Methods
	ConcreteDevice();
	~ConcreteDevice();
	void Setup();
	void Exit();
	void RegisterDriver(Driver* driver);
	void DeregisterDriver(Driver* driver);
	Driver* GetDriver(const char* name);
};

#endif // !__DEVICE_H__
