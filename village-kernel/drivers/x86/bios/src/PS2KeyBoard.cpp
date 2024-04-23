//###########################################################################
// PS2KeyBoard.cpp
// Definitions of the functions that manage key board
//
// $Copyright: Copyright (C) village
//###########################################################################
#include "Kernel.h"
#include "Hardware.h"


/// @brief PS2KeyBoard
class PS2KeyBoard : public Driver
{
private:
	//Members
	uint8_t keycode;
	InputEvent* inputevent;
	Interrupt*  interrupt;
	WorkQueue*  workQueue;
	WorkQueue::Work* work;
private:
	/// @brief Interrupt handler
	void InputHandler()
	{
		if (PortByteIn(PS2_READ_STATUS) & PS2_STATUS_OUTPUT_BUFFER)
		{
			keycode = PortByteIn(PS2_READ_DATA);
			workQueue->Sched(work);
		}
	}


	/// @brief Repor tkeycode and status
	void ReportHandler()
	{
		inputevent->ReportKey(keycode, 1);
	}
public:
	/// @brief Constructor
	PS2KeyBoard()
		:keycode(0),
		inputevent(NULL),
		interrupt(NULL),
		workQueue(NULL),
		work(NULL)
	{
	}


	/// @brief Destructor
	~PS2KeyBoard()
	{
	}


	/// @brief KeyBoard open
	bool Open()
	{
		//Get the inputevent pointer
		inputevent = &kernel->inputevent;

		//Get the interrupt pointer
		interrupt = &kernel->interrupt;

		//Get the work queue pointer
		workQueue = &kernel->workqueue;

		//Create work
		work = workQueue->Create((Method)&PS2KeyBoard::ReportHandler, this);

		//Set interrupt
		interrupt->SetISR(IRQ_Keyboard_Controller, (Method)(&PS2KeyBoard::InputHandler), this);

		return true;
	}


	/// @brief KeyBoard close
	void Close()
	{
		interrupt->RemoveISR(IRQ_Keyboard_Controller, (Method)(&PS2KeyBoard::InputHandler), this);
		workQueue->Delete(work);
	}
};


//Register driver
REGISTER_DRIVER(new PS2KeyBoard(), DriverID::_character, ps2keyboard);
