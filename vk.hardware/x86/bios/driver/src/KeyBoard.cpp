//###########################################################################
// KeyBoard.cpp
// Definitions of the functions that manage key board
//
// $Copyright: Copyright (C) village
//###########################################################################
#include "Input.h"
#include "Driver.h"
#include "Kernel.h"
#include "Hardware.h"
#include "WorkQueue.h"


/// @brief KeyBoard
class KeyBoard : public Driver
{
private:
	//Members
	uint8_t keycode;
	WorkQueue::Work* work;
private:
	/// @brief Interrupt handler
	void InputHandler()
	{
		if (PortByteIn(PS2_READ_STATUS) & PS2_STATUS_OUTPUT_BUFFER)
		{
			keycode = PortByteIn(PS2_READ_DATA);
			workQueue.Schedule(work);
		}
	}


	/// @brief Report handler
	void ReportHandler()
	{
		if (keycode >= 0 && keycode <= 0x39)
			input.ReportEvent(keycode, 1);
		else if (keycode >= 0x39 && keycode <= 0x39 + 0x80)
			input.ReportEvent(keycode - 0x80, 0);
	}
public:
	/// @brief Constructor
	KeyBoard()
	{
	}


	/// @brief Deconstructor
	~KeyBoard()
	{
	}


	/// @brief KeyBoard Initialize
	void Initialize()
	{
		interrupt.SetISR(IRQ_Keyboard_Controller, (Method)(&KeyBoard::InputHandler), this);
		work = workQueue.Create((Method)&KeyBoard::ReportHandler, this);
	}


	/// @brief KeyBoard Exit
	void Exit()
	{
		interrupt.RemoveISR(IRQ_Keyboard_Controller, (Method)(&KeyBoard::InputHandler), this);
		workQueue.Delete(work);
	}
};


//Register driver
REGISTER_DRIVER(new KeyBoard(), DriverID::_miscdev, keyboard);
