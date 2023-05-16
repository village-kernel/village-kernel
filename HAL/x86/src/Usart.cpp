//###########################################################################
// Usart.cpp
// Hardware layer class that manages a USART/Serial module
//
// $Copyright: Copyright (C) village
//###########################################################################
#include "Usart.h"


///Default Constructor
Usart::Usart()
	:channel(0)
{
}


///Initializes the serial module
void Usart::Initialize(uint16_t channel)
{
	switch (channel)
	{
		case 0: base = COM1; break;
		case 1: base = COM2; break;
		case 2: base = COM3; break;
		case 3: base = COM4; break;
		case 4: base = COM5; break;
		case 5: base = COM6; break;
		case 6: base = COM7; break;
		case 7: base = COM8; break;
		default: base = COM1; break;
	}

	PortByteOut(base + 1, 0x00);    // Disable all interrupts
	PortByteOut(base + 3, 0x80);    // Enable DLAB (set baud rate divisor)
	PortByteOut(base + 0, 0x00);    // Set divisor to 0 (lo byte) 115200 baud
	PortByteOut(base + 1, 0x00);    //                  (hi byte)
	PortByteOut(base + 3, 0x03);    // 8 bits, no parity, one stop bit
	PortByteOut(base + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
	PortByteOut(base + 4, 0x0B);    // IRQs enabled, RTS/DSR set
	PortByteOut(base + 4, 0x1E);    // Set in loopback mode, test the serial chip
	PortByteOut(base + 0, 0xAE);    // Test serial chip (send byte 0xAE and check if serial returns same byte)

	// Check if serial is faulty (i.e: not same byte as sent)
	if(PortByteIn(base + 0) != 0xAE) return;

	// If serial is not faulty set it in normal operation mode
	// (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
	PortByteOut(base + 4, 0x0F);
}


///Check if the send register is empty
bool Usart::IsTxRegisterEmpty()
{
	return (bool)(PortByteIn(base + COM_LINE_STATUS_Pos) & COM_LINE_STATUS_THRE);
}


///Write data to usart
int Usart::Write(uint8_t* txData, uint16_t length)
{
	while (length)
	{
		if (IsTxRegisterEmpty())
		{
			PortByteOut(base, *txData++);
			length--;
		}
	}

	return 0;
}


///Check if the read date register not empty
bool Usart::IsReadDataRegNotEmpty()
{
	return (bool)(PortByteIn(base + COM_LINE_STATUS_Pos) & COM_LINE_STATUS_DR);
}


///Read data from usart
int Usart::Read(uint8_t* rxData, uint16_t length)
{
	int count = 0;

	while (IsReadDataRegNotEmpty())
	{
		*rxData++ = PortByteIn(base);
		if (++count == length) break;
	}

	return count;
}
