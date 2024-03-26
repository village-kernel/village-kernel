//###########################################################################
// SpiFlash.cpp
//
// Communicates with an external flash using the SPI bus
//
// $Copyright: Copyright (C) village
//###########################################################################
#include "SpiFlash.h"
#include "HwConfig.h"
#include "Kernel.h"


///Static members initialize
bool SpiFlash::flashError = false;


///Initializes the spi flase module
bool SpiFlash::Open()
{
	//Init config
	InitConfig();

	//Initialize spi
	spi.Initialize(config.SpiCh);
	spi.ConfigModeAndPins(Spi::_Master, Spi::_Cpol0Cpha0, config.SpiConfig);
	spi.ConfigBaudRatePrescaler(Spi::_Fpclk16);
	spi.ConfigFrame(Spi::_MsbFirst, Spi::_8Bit);
	spi.Enable();

	//Initialize spi flash cs pin
	csPin.Initialize(config.flashcsCh, config.flashcsPin, Gpio::_High);

	//Initialize spi flash wp pin
	wpPin.Initialize(config.flashwpCh, config.flashwpPin, Gpio::_High);

	//Check if the device id is correct
	if (GetDeviceID() != DeviceID) flashError = true;

	return !flashError;
}


/// @brief Close
void SpiFlash::Close()
{

}


///Init config
inline void SpiFlash::InitConfig()
{
	config.SpiCh = SPI_FLASH_CHANNEL;
	config.SpiConfig.sckCh = SPI_FLASH_SCK_CH;
	config.SpiConfig.sckPin = SPI_FLASH_SCK_PIN;
	config.SpiConfig.sckAltNum = SPI_FLASH_SCK_AF_NUM;
	config.SpiConfig.misoCh = SPI_FLASH_MISO_CH;
	config.SpiConfig.misoPin = SPI_FLASH_MISO_PIN;
	config.SpiConfig.misoAltNum = SPI_FLAHS_MISO_AF_NUM;
	config.SpiConfig.mosiCh = SPI_FLASH_MOSI_CH;	
	config.SpiConfig.mosiPin = SPI_FLASH_MOSI_PIN;
	config.SpiConfig.mosiAltNum = SPI_FLASH_MOSI_AF_NUM;
	config.flashcsCh = SPI_FLASH_CS_CH;
	config.flashcsPin = SPI_FLASH_CS_PIN;
	config.flashwpCh = SPI_FLASH_WP_CH;
	config.flashwpPin = SPI_FLASH_WP_PIN;
}


///Spi flash write writeAddr
inline void SpiFlash::WriteAddr(uint32_t addr)
{
	spi.WriteAndReadOneByte(addr >> 16);
	spi.WriteAndReadOneByte(addr >> 8);
	spi.WriteAndReadOneByte(addr);
}


///Spi flash write cmd
inline void SpiFlash::WriteCmd(uint8_t cmd)
{
	spi.WriteAndReadOneByte(cmd);
}


///Spi flash write one byte writeData
inline void SpiFlash::WriteOneByte(uint8_t data)
{
	spi.WriteAndReadOneByte(data);
}


///Spi flash read one byte writeData
inline uint8_t SpiFlash::ReadOneByte()
{
	return spi.WriteAndReadOneByte(0x00);
}


///Spi flash Select chip
inline void SpiFlash::SelectChip()
{
	csPin.Clear();
}


///Spi flash Unselect chip
inline void SpiFlash::UnselectChip()
{
	csPin.Set();
}


///Spi flash Get ID
uint16_t SpiFlash::GetDeviceID()
{
	SelectChip();

	WriteCmd(_ReadManufacturerDeviceID);
	WriteAddr(0);

	//manufacturer id and device id
	uint16_t id = ReadOneByte() << 8 | (uint16_t)ReadOneByte();

	UnselectChip();
	return id;
}


///Spi flash chip enable
void SpiFlash::WriteEnable()
{
	SelectChip();

	WriteCmd(_WriteEnable);

	UnselectChip();
}


///Spi flash chip disable
void SpiFlash::WriteDisable()
{
	SelectChip();

	WriteCmd(_WriteDisable);

	UnselectChip();
}


///Spi flash check busy
void SpiFlash::WaitForBusy()
{
	SelectChip();

	WriteCmd(_ReadStatusRegister);

	while ((ReadOneByte() & 0x01)) {}

	UnselectChip();
}


///Spi flash chip erase
void SpiFlash::EraseChip()
{
	WriteEnable();
	SelectChip();

	WriteCmd(_ChipErase);

	UnselectChip();
	WaitForBusy();
}


///Spi flash sector erase
void SpiFlash::EraseSector(uint32_t address)
{
	WriteEnable();
	SelectChip();

	WriteCmd(_SectorErase);
	WriteAddr(address);

	UnselectChip();
	WaitForBusy();
}


///Spi flash page write
void SpiFlash::PageWriteBytes(uint8_t* txData, uint16_t size, uint32_t address)
{
	WriteEnable();
	SelectChip();

	WriteCmd(_PageProgram);
	WriteAddr(address);

	for (uint16_t i = 0; i < size; i++)
	{
		WriteOneByte(txData[i]);
	}

	UnselectChip();
	WaitForBusy();
}


///Spi flash sector write
void SpiFlash::SectorWriteBytes(uint8_t* txData, uint16_t size, uint32_t address)
{
	uint16_t bytesRemain = size;
	uint32_t writeAddr = address;
	uint8_t* writeData = txData;

	while (bytesRemain)
	{
		//Calculate the size of the write data
		uint16_t writeSize = OnePageByteSize - (uint8_t)writeAddr;
		if (bytesRemain < writeSize) writeSize = bytesRemain;

		//Write bytes
		PageWriteBytes(writeData, writeSize, writeAddr);
		bytesRemain -= writeSize;
		writeAddr += writeSize;
		writeData += writeSize;
	}
}


///Writes a specified number of bytes of writeData into provided address
int SpiFlash::WriteAnywhere(uint8_t *txData, uint32_t size, uint32_t address)
{
	//return if there are any problems with the flash;
	if (flashError) return -1;

	uint32_t bytesRemain = size;
	uint32_t writeAddr = address;
	uint8_t* writeData = txData;
	uint8_t  buffer[OneSectorByteSize] = { 0 };

	while (bytesRemain)
	{
		//Calculate the size of the write data
		uint32_t sectorStartAddr = writeAddr >> 12 << 12;
		uint32_t sectorWriteAddr = writeAddr & (OneSectorByteSize - 1);
		uint32_t sectorWriteSize = OneSectorByteSize - sectorWriteAddr;
		if (bytesRemain < sectorWriteSize) sectorWriteSize = bytesRemain;

		//Read sector data to buffer
		Read(buffer, OneSectorByteSize, sectorStartAddr);
		
		//Erase sector
		EraseSector(sectorStartAddr);

		//Integrate data into the buffer
		for (uint32_t i = 0; i < sectorWriteSize; i++)
		{
			buffer[sectorWriteAddr + i] = writeData[i];
		}

		//Calculate write address offset
		uint32_t offset = 0;
		for (uint32_t i = 0; i < OneSectorByteSize; i++)
		{
			if (buffer[i] != 0xff)
			{
				offset = i;
				break;
			}
		}

		//Calculate the size of the write data
		uint32_t writeSize = 0;
		for (uint32_t i = OneSectorByteSize; i > 0; i--)
		{
			if (buffer[i - 1] != 0xff)
			{
				writeSize = i - offset;
				break;
			}
		}

		//Write data into sector
		SectorWriteBytes(buffer + offset, writeSize, sectorStartAddr + offset);

		//Update bytes reamin, write address and write data
		bytesRemain -= sectorWriteSize;
		writeAddr += sectorWriteSize;
		writeData += sectorWriteSize;
	}

	return size;
}


///Writes a specified number of bytes of writeData into provided address
int SpiFlash::Write(uint8_t *txData, uint32_t blkSize, uint32_t sector)
{
	//return if there are any problems with the flash;
	if (flashError) return -1;

	//Adapte fatfs
	uint32_t bytesRemain = blkSize * sector_size;
	uint32_t writeAddr = sector * sector_size;
	uint8_t* writeData = txData;

	while (bytesRemain)
	{
		//Erase sector
		if (0 == (writeAddr % OneSectorByteSize))
		{
			EraseSector(writeAddr);
		}

		//Calculate the size of the write data
		uint16_t writeSize = OnePageByteSize - (uint8_t)writeAddr;
		if (bytesRemain < writeSize) writeSize = bytesRemain;

		//Write bytes
		PageWriteBytes(writeData, writeSize, writeAddr);
		bytesRemain -= writeSize;
		writeAddr += writeSize;
		writeData += writeSize;
	}

	return flashError ? -1 : 0;
}


///Reads a specified number of bytes of writeData into the provided address
int SpiFlash::Read(uint8_t* rxData, uint32_t blkSize, uint32_t sector)
{
	//return if there are any problems with the flash;
	if (flashError) return -1;

	//Adapte fatfs
	uint32_t readAddr = sector * sector_size;
	uint32_t readSize = blkSize * sector_size;

	SelectChip();

	WriteCmd(_ReadData);
	WriteAddr(readAddr);

	for (uint32_t i = 0; i < readSize; i++)
	{
		rxData[i] = ReadOneByte();
	}

	UnselectChip();

	return flashError ? -1 : 0;;
}


///Spi flash io ctrl
int SpiFlash::IOCtrl(uint8_t cmd, void* data)
{
	switch (cmd)
	{
		case _GetSectorCount:
			*(uint32_t*)data = sector_count;
			break;
		case _GetSectorSize:
			*(uint16_t *)data = sector_size;
			break;
		case _GetBlockSize:
			*(uint16_t *)data = block_size;
		default: break;
	}
	return 0;
}


///Register driver
REGISTER_DRIVER(new SpiFlash(), DriverID::_block, spiFlash);
