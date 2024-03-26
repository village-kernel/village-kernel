//###########################################################################
// Debug.h
// Declarations of the debug
//
// $Copyright: Copyright (C) village
//###########################################################################
#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "Kernel.h"
#include "Mutex.h"
#include "DrvStream.h"


/// @brief ConcreteDebug
class ConcreteDebug : public Debug
{
private:
	//Static constants
	static const uint16_t buf_size = 100;

	//Members
	DrvStream transceiver;
	Mutex     mutex;
	char      data[buf_size] = { 0 };
	char      txBuffer[buf_size] = { 0 };
	uint16_t  txBufPos;
	uint8_t   debugLevel;

	//Methods
	void Write(const char* data);
	void Sending();
public:
	//Methods
	ConcreteDebug();
	~ConcreteDebug();
	void Setup();
	void Exit();
	void Log(const char* format, ...);
	void Info(const char* format, ...);
	void Error(const char* format, ...);
	void Warn(const char* format, ...);
	void Output(int level, const char* format, ...);
	void SetDebugLevel(int level);
};

#endif //!__DEBUG_H__
