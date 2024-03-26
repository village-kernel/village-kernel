 //###########################################################################
// GeneralTimer.cpp
// Definitions of the functions that manage general timer
//
// $Copyright: Copyright (C) village
//###########################################################################
#include "Driver.h"
#include "Kernel.h"
#include "Hardware.h"


/// @brief GeneralTimer
class GeneralTimer : public Driver
{
private:
	//Members
	uint32_t timer;
	uint32_t TIMERX[3] = {TIMER_CH0, TIMER_CH1, TIMER_CH2};
public:
	/// @brief Constructor
	/// @param timer 
	GeneralTimer(uint32_t timer = 0)
		:timer(0)
	{
		if (timer >= 0 && timer <= 2) this->timer = timer;
	}


	/// @brief Open
	bool Open()
	{
		//uint32_t freq = 1000; //1000hz, 1ms
		//uint32_t divider = 1193182 / freq;
		//uint8_t low  = low_8(divider);
		//uint8_t high = high_8(divider);

		//PortByteOut(TIMERX[timer], 0x36); //Command port
		//PortByteOut(TIMERX[timer], low);
		//PortByteOut(TIMERX[timer], high);
	}


	/// @brief Close
	void Close()
	{

	}
};


///Register driver
REGISTER_DRIVER(new GeneralTimer(), DriverID::_miscellaneous, timer0);
