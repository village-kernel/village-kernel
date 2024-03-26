//###########################################################################
// DrvStream.h
// Declarations of the functions that manage file
//
// $Copyright: Copyright (C) village
//###########################################################################
#ifndef __DRV_STREAM_H__
#define __DRV_STREAM_H__

#include "FileDefs.h"
#include "Driver.h"


/// @brief DrvStream
class DrvStream
{
private:
	//Members
	Driver* driver;
public:
	//Methods
	DrvStream(const char* name = NULL, int mode = 0);
	~DrvStream();
	bool Open(const char* name, int mode = 0);
	int Write(char* data, int size, int offset = 0);
	int Read(char* data, int size, int offset = 0);
	int IOCtrl(uint8_t cmd, void* data);
	void Close();
};

#endif //!__DRV_STREAM_H__
