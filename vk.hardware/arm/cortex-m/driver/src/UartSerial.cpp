//###########################################################################
// UartSerial.cpp
// Provides a simple interface to transmit and receive bytes via the USART 
// module. DMA is used to minimize CPU usage. All buffers are managed 
// by the class internally.
//
// $Copyright: Copyright (C) village
//###########################################################################
#include "Kernel.h"
#include "UartSerial.h"
#include "HwConfig.h"
#include "string.h"


///Initializes internal buffers
bool UartSerial::Open()
{
	//Init config
	InitConfig();

	//Configure usart
	usart.Initialize(config.usartCh);
	usart.ConfigPin(config.usartTxPin);
	usart.ConfigPin(config.usartRxPin);
	if (config.enableRTS) usart.ConfigPin(config.usartDePin);
	usart.SetBaudRate(config.usartBaudrate);
	usart.ConfigPortSettings(Usart::_8Bits, Usart::_NoParity, Usart::_1Stop);
	usart.ConfigDriverEnableMode(true, false);
	usart.ConfigReceiverTimeout(true, 39); // 3.5bytes: 3.5 * 11 = 38.5
	usart.ConfigDma();
	usart.Enable();

	//Configure rx fifo
	rxFifo.Initialize(config.usartRxDmaGroup, config.usartRxDmaCh, config.usartRxDmaReq, usart.GetRxAddr());
	rxFifo.Enable();
	rxFifo.Clear();

	//Configure tx dma
	txDma.Initialize(config.usartTxDmaGroup, config.usartTxDmaCh);
	txDma.ConfigDirAndDataWidth(Dma::_MemoryToPeriph, Dma::_8Bits);
	txDma.ConfigPriority(Dma::_VeryHigh);
	txDma.SetPeriphAddr(usart.GetTxAddr());
	txDma.ConfigIncMode(true, false);
	txDma.ConfigRequest(config.usartTxDmaReq);

	return true;
}


/// @brief Close
void UartSerial::Close()
{

}


///Initialize config
void UartSerial::InitConfig()
{
	//Config uart serial
	config.usartCh = UART_SERIAL_CHANNEL;
	config.usartBaudrate = UART_SERIAL_BAUD_RATE;

	//Config uart rx pin
	config.usartRxPin.ch = UART_SERIAL_RX_CH;
	config.usartRxPin.pin = UART_SERIAL_RX_PIN;
	config.usartRxPin.alt = UART_SERIAL_RX_AF_NUM;

	//Config uart tx pin
	config.usartTxPin.ch = UART_SERIAL_TX_CH;
	config.usartTxPin.pin = UART_SERIAL_TX_PIN;
	config.usartTxPin.alt = UART_SERIAL_TX_AF_NUM;

	//Config uart rx dma
	config.usartRxDmaGroup = UART_SERIAL_RX_DMA_GROUP;
	config.usartRxDmaCh = UART_SERIAL_RX_DMA_CHANNEL;
	config.usartRxDmaReq = UART_SERIAL_RX_DMA_REQUEST;

	//Config uart tx dma
	config.usartTxDmaGroup = UART_SERIAL_TX_DMA_GROUP;
	config.usartTxDmaCh = UART_SERIAL_TX_DMA_CHANNEL;
	config.usartTxDmaReq = UART_SERIAL_TX_DMA_REQUEST;
}


///Copy the sent data to the send buffer to avoid accidentally modifying the sent data.
inline void UartSerial::CopyTxData(uint8_t* txData, uint16_t length)
{
	memcpy((uint8_t*)txBuffer, (uint8_t*)txData, length);
}


///Write data via dma.
int UartSerial::Write(uint8_t* data, uint32_t size, uint32_t offset)
{
	usart.CheckError();

	if (txDma.IsReady())
	{
		CopyTxData(data + offset, size);

		//Reset pipeline, sync bus and memory access
		__asm ("dmb\n" "dsb\n" "isb\n");

		txDma.Disable();
		txDma.ClearTransferCompleteFlag();
		txDma.SetMemAddr0(txBuffer);
		txDma.SetDataLen(size);
		txDma.Enable();

		return size;
	}

	return 0;
}


///Read data from rx buffer
int UartSerial::Read(uint8_t* data, uint32_t size, uint32_t offset)
{
	uint32_t readSize = 0;

	usart.CheckError();

	rxFifo.Update();

	while (rxFifo.Length())
	{
		data[offset + readSize++] = rxFifo.Dequeue();

		if (readSize >= size) break;
	}

	return readSize;
}


///Register driver
REGISTER_DRIVER(new UartSerial(), DriverID::_character, serial0);
