//###########################################################################
// DrvStream.cpp
// Definitions of the functions that manage file
//
// $Copyright: Copyright (C) village
//###########################################################################
#include "DrvStream.h"
#include "Kernel.h"


/// @brief Constructor
DrvStream::DrvStream(const char* name, int mode)
	:driver(NULL)
{
	if (NULL != name) Open(name, mode);
}


/// @brief Destructor
DrvStream::~DrvStream()
{
}


/// @brief DrvStream open
/// @param name 
/// @param mode 
/// @return 
bool DrvStream::Open(const char* name, int mode)
{
	if (NULL == driver)
	{
		driver = kernel->device.GetDriver(name);
		return driver->Open();
	}
	return false;
}


/// @brief DrvStream write
/// @param data 
/// @param size 
/// @param offset 
/// @return 
int DrvStream::Write(char* data, int size, int offset)
{
	if (NULL != driver)
	{
		return driver->Write((uint8_t*)data, size, offset);
	}
	return 0;
}


/// @brief DrvStream read
/// @param data 
/// @param size 
/// @param offset 
/// @return 
int DrvStream::Read(char* data, int size, int offset)
{
	if (NULL != driver)
	{
		return driver->Read((uint8_t*)data, size, offset);
	}
	return 0;
}


/// @brief DrvStream io ctrl
/// @param cmd 
/// @param data 
/// @return 
int DrvStream::IOCtrl(uint8_t cmd, void* data)
{
	if (NULL != driver)
	{
		return driver->IOCtrl(cmd, data);
	}
	return 0;
}


/// @brief DrvStream close
void DrvStream::Close()
{
	if (NULL != driver)
	{
		driver->Close();
	}
}
