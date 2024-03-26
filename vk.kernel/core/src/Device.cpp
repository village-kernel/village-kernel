//###########################################################################
// Device.cpp
// Definitions of the functions that manage driver
//
// $Copyright: Copyright (C) village
//###########################################################################
#include "Device.h"


/// @brief Constructor
ConcreteDevice::ConcreteDevice()
{
}


/// @brief Destructor
ConcreteDevice::~ConcreteDevice()
{
}


/// @brief Device Setup
void ConcreteDevice::Setup()
{
	
}


/// @brief Device Exit
void ConcreteDevice::Exit()
{
	drivers.Release();
}


/// @brief Register driver object
/// @param driver driver pointer
void ConcreteDevice::RegisterDriver(Driver* driver)
{
	drivers.Add(driver, driver->GetName());
}


/// @brief Deregister driver object
/// @param driver driver pointer
void ConcreteDevice::DeregisterDriver(Driver* driver)
{
	drivers.Remove(driver);
}


/// @brief Get the driver object by name
/// @param name driver name
/// @return driver
Driver* ConcreteDevice::GetDriver(const char* name)
{
	return drivers.GetItemByName(name);
}
