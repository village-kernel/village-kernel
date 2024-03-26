//###########################################################################
// SerialPort.cpp
// Definitions of the functions that manage serial port
//
// $Copyright: Copyright (C) village
//###########################################################################
#include "string.h"
#include "Driver.h"
#include "Kernel.h"
#include "Hardware.h"


/// @brief SerialPort
class SerialPort : public Driver
{
public:
	//Enumerations
	enum DataBits
	{
		_5Bits = 0,
		_6Bits = 1,
		_7Bits = 2,
		_8Bits = 3,
	};

	enum Parity
	{
		_NoParity = 0,
		_ODDParity = 1,
		_EvenParity = 2,
		_MarkParity = 3,
		_SpaceParity = 4,
	};

	enum StopBits
	{
		_1Stop = 0,
		_1AndHalfStop = 1,
	};
private:
	//Members
	uint32_t port;
	uint32_t COMX[8] = {COM1, COM2, COM3, COM4, COM5, COM6, COM7 };
private:
	///Check if the send register is empty
	bool IsTxRegisterEmpty()
	{
		return (bool)(PortByteIn(COMX[port] + COM_LINE_STATUS_Pos) & COM_LINE_STATUS_THRE);
	}
	

	///Check if the read date register not empty
	bool IsReadDataRegNotEmpty()
	{
		return (bool)(PortByteIn(COMX[port] + COM_LINE_STATUS_Pos) & COM_LINE_STATUS_DR);
	}
public:
	/// @brief Constructor
	/// @param port 
	SerialPort(uint32_t port = 0)
		:port(0)
	{
		if (port >= 0 && port <= 7) this->port = port;
	}


	/// @brief Open
	bool Open()
	{
		//Setup serial
		PortByteOut(COMX[port] + 1, 0x00);    // Disable all interrupts
		PortByteOut(COMX[port] + 3, 0x80);    // Enable DLAB (set baud rate divisor)
		PortByteOut(COMX[port] + 0, 0x00);    // Set divisor to 0 (lo byte) 115200 baud
		PortByteOut(COMX[port] + 1, 0x00);    //                  (hi byte)
		PortByteOut(COMX[port] + 3, 0x03);    // 8 bits, no parity, one stop bit
		PortByteOut(COMX[port] + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
		PortByteOut(COMX[port] + 4, 0x0B);    // IRQs enabled, RTS/DSR set
		PortByteOut(COMX[port] + 4, 0x1E);    // Set in loopback mode, test the serial chip
		PortByteOut(COMX[port] + 0, 0xAE);    // Test serial chip (send byte 0xAE and check if serial returns same byte)

		//Check if serial is faulty (i.e: not same byte as sent)
		if(PortByteIn(COMX[port] + 0) != 0xAE) return false;

		//If serial is not faulty set it in normal operation mode
		//(not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
		PortByteOut(COMX[port] + 4, 0x0F);

		return true;
	}


	/// @brief Write data
	/// @param data 
	/// @param size 
	/// @param offset 
	/// @return 
	int Write(uint8_t* data, uint32_t size = 0, uint32_t offset = 0)
	{
		int count = size;

		while (size)
		{
			if (IsTxRegisterEmpty())
			{
				PortByteOut(COMX[port], *data++);
				size--;
			}
		}

		return count;
	}


	/// @brief Read data from rx buffer
	/// @param data 
	/// @param size 
	/// @param offset 
	/// @return 
	int Read(uint8_t* data, uint32_t size = 0, uint32_t offset = 0)
	{
		uint32_t count = 0;

		while (IsReadDataRegNotEmpty())
		{
			*data++ = PortByteIn(COMX[port]);
			if (++count >= size) break;
		}

		return count;
	}


	/// @brief Close
	void Close()
	{

	}
};


///Register driver
REGISTER_DRIVER(new SerialPort(), DriverID::_character, serial0);
