//###########################################################################
// Debug.cpp
// Definitions of the functions that manage debug
//
// $Copyright: Copyright (C) village
//###########################################################################
#include "string.h"
#include "stdarg.h"
#include "stdio.h"
#include "Debug.h"


/// @brief Constructor
ConcreteDebug::ConcreteDebug()
	:txBufPos(0),
	debugLevel(_Lv2)
{
}


/// @brief Destructor
ConcreteDebug::~ConcreteDebug()
{
}


/// @brief Setup
void ConcreteDebug::Setup()
{
	//Get transceiver driver and initialize
	transceiver.Open("serial0", FileMode::_Write);
}


/// @brief Exit
void ConcreteDebug::Exit()
{
	transceiver.Close();
}


/// @brief Write
/// @param data 
void ConcreteDebug::Write(const char* data)
{
	//Calculate the string length
	int size = strlen((const char*)data);

	//Copy msg data into txBuffer
	for (int i = 0; i < size; i++)
	{
		txBuffer[txBufPos++] = data[i];

		//The txBuffer is full, block here until the data is sent
		if (txBufPos >= buf_size) Sending();
	}

	//Sending msg
	Sending();
}


/// @brief Sent data when txbuffer not empty
/// @brief Reset txBufPos when sent data successfully
void ConcreteDebug::Sending()
{
	if (txBufPos)
	{
		while (!transceiver.Write(txBuffer, txBufPos)) {}
		txBufPos = 0;
	}
}


/// @brief Debug log
/// @param format 
/// @param  
void ConcreteDebug::Log(const char* format, ...)
{
	mutex.Lock();
	va_list arg;
	va_start(arg, format);
	vsprintf(data, format, arg);
	va_end(arg);
	Write("Log: ");
	Write(data);
	Write("\r\n");
	mutex.Unlock();
}


/// @brief Debug info
/// @param format 
/// @param  
void ConcreteDebug::Info(const char* format, ...)
{
	mutex.Lock();
	va_list arg;
	va_start(arg, format);
	vsprintf(data, format, arg);
	va_end(arg);
	Write("\033[36mInfo: ");
	Write(data);
	Write("\r\n\033[39m");
	mutex.Unlock();
}


/// @brief Debug error
/// @param format 
/// @param  
void ConcreteDebug::Error(const char* format, ...)
{
	mutex.Lock();
	va_list arg;
	va_start(arg, format);
	vsprintf(data, format, arg);
	va_end(arg);
	Write("\033[31mError: ");
	Write(data);
	Write("\r\n\033[39m");
	mutex.Unlock();
}


/// @brief Debug warn
/// @param format 
/// @param  
void ConcreteDebug::Warn(const char* format, ...)
{
	mutex.Lock();
	va_list arg;
	va_start(arg, format);
	vsprintf(data, format, arg);
	va_end(arg);
	Write("\033[33mWarning: ");
	Write(data);
	Write("\r\n\033[39m");
	mutex.Unlock();
}


/// @brief Debug output
/// @param format 
/// @param  
void ConcreteDebug::Output(int level, const char* format, ...)
{
	if (level >= debugLevel)
	{
		mutex.Lock();
		va_list arg;
		va_start(arg, format);
		vsprintf(data, format, arg);
		va_end(arg);
		Write("\033[36mDebug: ");
		Write(data);
		Write("\r\n\033[39m");
		mutex.Unlock();
	}
}


/// @brief Debug set debug level
/// @param level 
void ConcreteDebug::SetDebugLevel(int level)
{
	if (level >= _Lv0 && level <= _Lv5)
	{
		debugLevel = level;
	}
	else
	{
		Error("The level %d out of debug level", level);
	}
}
