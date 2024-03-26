//###########################################################################
// SdCard.cpp
//
// Communicates with sd card using the SPI bus
//
// $Copyright: Copyright (C) village
//###########################################################################
#include "SdCard.h"
#include "HwConfig.h"
#include "Kernel.h"


///Sd card initialize
bool SdCard::Open()
{
	//Init config
	InitConfig();

	//Initialize spi
	spi.Initialize(config.SpiCh);
	spi.ConfigModeAndPins(Spi::_Master, Spi::_Cpol1Cpha1, config.SpiConfig);
	spi.ConfigBaudRatePrescaler(Spi::_Fpclk16);
	spi.ConfigFrame(Spi::_MsbFirst, Spi::_8Bit);
	spi.Enable();

	//Initialize spi sd cs pin
	csPin.Initialize(config.sdcsCh, config.sdcsPin, Gpio::_High);

	//Initialize sd card detect pin
	detectPin.Initialize(config.detectCh, config.detectPin);

	//Initialize sd card
	SdCardInit();

	return true;
}


/// @brief Close
void SdCard::Close()
{

}


///Init config
inline void SdCard::InitConfig()
{
	config.SpiCh = SPI_SD_CHANNEL;
	config.SpiConfig.sckCh = SPI_SD_SCK_CH;
	config.SpiConfig.sckPin = SPI_SD_SCK_PIN;
	config.SpiConfig.sckAltNum = SPI_SD_SCK_AF_NUM;
	config.SpiConfig.misoCh = SPI_SD_MISO_CH;
	config.SpiConfig.misoPin = SPI_SD_MISO_PIN;
	config.SpiConfig.misoAltNum = SPI_SD_MISO_AF_NUM;
	config.SpiConfig.mosiCh = SPI_SD_MOSI_CH;	
	config.SpiConfig.mosiPin = SPI_SD_MOSI_PIN;
	config.SpiConfig.mosiAltNum = SPI_SD_MOSI_AF_NUM;
	config.sdcsCh = SPI_SD_CS_CH;
	config.sdcsPin = SPI_SD_CS_PIN;
	config.detectCh = SPI_SD_DETECT_CH;
	config.detectPin = SPI_SD_DETECT_PIN;
}


///Spi sd card write one byte writeData
inline void SdCard::WriteOneByte(uint8_t data)
{
	spi.WriteAndReadOneByte(data);
}


///Spi sd card read one byte writeData
inline uint8_t SdCard::ReadOneByte()
{
	return spi.WriteAndReadOneByte(0xff);
}


///Spi sd card wait ready
inline uint8_t SdCard::WaitReady()
{
	for (uint16_t cnt = 0; cnt < 0xffff; cnt++)
	{
		if (0xff == ReadOneByte()) return 0;
	}
	return 1;
}


///Get sd card ack
inline uint8_t SdCard::GetAck(uint8_t reponse)
{
	for (uint32_t cnt = 0; cnt < 0xffff; cnt++)
	{
		if (ReadOneByte() == reponse) return _response_no_err;
	}
	return _response_failure;
}


///Spi select sd card
inline uint8_t SdCard::SelectCard()
{
	csPin.Clear();
	if (0 == WaitReady()) return 0;
	UnselectCard();
	return 1;
}


///Spi unselect sd card
inline uint8_t SdCard::UnselectCard()
{
	csPin.Set();
	ReadOneByte(); //Provide additional 8 clocks
	return 0;
}


///Spi sd card write cmd
inline uint8_t SdCard::WriteCmd(uint8_t cmd, uint32_t arg, uint8_t crc)
{
	const uint8_t write_separately = 0x40;

	//Release bus and select sd card
	UnselectCard();
	if (SelectCard()) return 0xff;

	//Send cmd, arg and crc
	WriteOneByte(cmd | write_separately);
	WriteOneByte((uint8_t)(arg >> 24));
	WriteOneByte((uint8_t)(arg >> 16));
	WriteOneByte((uint8_t)(arg >> 8));
	WriteOneByte((uint8_t)(arg));
	WriteOneByte(crc);

	//Wait response
	if (_StopTrans == cmd) ReadOneByte();

	uint8_t res = 0;

	for (uint8_t cnt = 0; cnt < 0x1f; cnt++)
	{
		res = ReadOneByte();
		if (0 == (res & _com_crc_err)) return res; 
	}

	return res;
}


///Sd card send data
inline uint8_t SdCard::SendData(uint8_t *txData, uint16_t size, uint8_t cmd)
{
	if (WaitReady()) return 1;

	WriteOneByte(cmd);

	if (0xfd != cmd)
	{
		for (uint16_t cnt = 0; cnt < 512; cnt++)
		{
			if (cnt < size)
			{
				WriteOneByte(txData[cnt]);
			}
			else
			{
				WriteOneByte(0xff);
			}
		}
		//ignore crc
		WriteOneByte(0xff);
		WriteOneByte(0xff);
		
		//Get response
		uint8_t res = ReadOneByte();
		if (_data_ok != (res & 0x1f)) return 2;
	}
	return 0;
}


///Sd card receive data
inline uint8_t SdCard::RecvData(uint8_t *rxData, uint16_t size)
{
	if (GetAck(0xfe)) return 1;

	for (uint16_t cnt = 0; cnt < size; cnt++)
	{
		rxData[cnt] = ReadOneByte();
	}

	//ignore crc
	WriteOneByte(0xff);
	WriteOneByte(0xff);
	return 0;
}


///Spi sd card initialize
uint8_t SdCard::SdCardInit()
{
	uint8_t res;
	uint8_t data[4];

	//Release bus
	for(uint8_t i = 0; i < 10; i++)
	{
		WriteOneByte(0xff);
	}

	//Go to idle status
	for (uint8_t i = 0; i < 20; i++)
	{
		res = WriteCmd(_Reset, 0, 0x95);
		if (_in_idle_state == res) break;
	}

	//Set sd card type
	sdcardType = _NONE;

	if (_in_idle_state == res)
	{
		if (1 == WriteCmd(_SendIfCond, 0x1aa, 0x87)) //sd v2.0
		{
			for (uint8_t i = 0; i < 4; i++)
			{
				data[i] = ReadOneByte();
			}

			if ((0x01 == data[2]) && (0xaa == data[3])) //card support 2.7v~3.6v
			{
				for (uint16_t i = 0; i < 0xffff; i++)
				{
					res = WriteCmd(_AppCmd, 0, 0x01);
					res = WriteCmd(_SdSendOpCond, 0x40000000, 0x01);

					if (_response_no_err == res) break;
				}

				if (_response_no_err == res)
				{
					res = WriteCmd(_ReadOCR, 0, 0x01);

					if (_response_no_err == res)
					{
						for (uint8_t i = 0; i < 4; i++)
						{
							data[i] = ReadOneByte();
						}
		
						sdcardType = (data[0] & 0x40) ? _V2HC : _V2;
					}
				}
			}
		}
		else //SD v1.x /mmc v3
		{
			uint16_t i = 0;

			res = WriteCmd(_AppCmd, 0, 0x01);
			res = WriteCmd(_SdSendOpCond, 0, 0x01);

			if (res <= 1)
			{
				sdcardType = _V1;
				for (i = 0; i < 0xffff; i++)
				{
					res = WriteCmd(_AppCmd, 0, 0x01);
					res = WriteCmd(_SdSendOpCond, 0, 0x01);

					if (_response_no_err == res) break;
				}
			}
			else
			{
				sdcardType = _MMC;
				for (i = 0; i < 0xffff; i++)
				{
					res = WriteCmd(_GetOCR, 0, 0x01);
					if (_response_no_err == res) break;
				}
			}

			if ((0xffff == i) || (_response_no_err != WriteCmd(_SetBlockLen, 512, 0x01)))
			{
				sdcardType = _NONE;
			}
		}
	}

	UnselectCard();

	return (_NONE != sdcardType) ? 0 : res;
}


///Spi sd card get ocr data
int SdCard::GetOCR(uint8_t* ocrData)
{
	uint8_t	res = WriteCmd(_ReadOCR, 0, 0x01);

	if (_response_no_err == res)
	{
		for (uint8_t i = 0; i < 4; i++)
		{
			ocrData[i] = ReadOneByte();
		}
	}

	return res ? 1 : 0;
}


///Spi sd card get cid data
int SdCard::GetCID(uint8_t* cidData)
{
	uint8_t res = WriteCmd(_SendCID, 0, 0x01);
	
	if (_response_no_err == res)
	{
		res = RecvData(cidData, 16);
	}
	
	UnselectCard();

	return res ? 1 : 0;
}


///Spi sd card get csd data
int SdCard::GetCSD(uint8_t* csdData)
{
	uint8_t res = WriteCmd(_SendCSD, 0, 0x01);
	
	if (_response_no_err == res)
	{
		res = RecvData(csdData, 16);
	}
	
	UnselectCard();

	return res ? 1 : 0;
}


///Spi sd card get sector count
uint32_t SdCard::GetSectorCount()
{
	uint8_t csd[16];
	
	if (0 != GetCSD(csd)) return 0;

	//V2.00 card, sdhc
	if (0x40 == (csd[0]& 0xc0))
	{
		uint16_t size = csd[9] + ((uint16_t)csd[8] << 8) + 1;
		return (uint32_t)size << 10;
	}
	//V1.xx card
	else
	{
		uint8_t num = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
		uint16_t size = (csd[8] >> 6) + ((uint16_t)csd[7] << 2) + ((uint16_t)(csd[6] & 3) << 10) + 1;
		return (uint32_t)size << (num - 9);
	}
	return 0;
}


///Spi sd card Sync
int SdCard::Sync()
{
	uint8_t res = 1;
	csPin.Clear();
	if (0 == WaitReady()) res = 0;
	csPin.Set();
	return res;
}


///Spi sd card io ctrl
int SdCard::IOCtrl(uint8_t cmd, void* data)
{
	switch (cmd)
	{
		case _CtrlSync: 
			Sync();
			break;
		case _GetSectorCount:
			*(uint32_t*)data = GetSectorCount();
			break;
		case _GetSectorSize:
			*(uint16_t *)data = (SdCardType::_V2HC != sdcardType) ? 512 : 1;
			break;
		case _GetBlockSzie:
			*(uint16_t *)data = 8;
			break;
		case _Ctrltrim:
			break;
		
		case _GetType:
			*(uint32_t *)data = 1;
			break;
		case _GetCSDData:
			GetCSD((uint8_t*)data);
			break;
		case _GetCIDData:
			GetCID((uint8_t*)data);
			break;
		case _GetOCRData:
			GetOCR((uint8_t*)data);
			break;
		case _GetSdstat:
			*(uint32_t *)data = 0;
		default: break;
	}
	return 0;
}


///Spi sd card write data
int SdCard::Write(uint8_t *txData, uint32_t blkSize, uint32_t sector)
{
	uint8_t res = 0;

	if (SdCardType::_V2HC != sdcardType) sector = sector * 512;

	if (1 == blkSize)
	{
		res = WriteCmd(_WriteBlock, sector, 0x01);

		if (_response_no_err == res)
		{
			res = SendData(txData, 512, 0xfe);
		}
	}
	else
	{
		if (SdCardType::_MMC != sdcardType)
		{
			WriteCmd(_AppCmd, 0, 0x01);
			WriteCmd(_SetWrBlkEraseCount, blkSize, 0x01);
		}

		res = WriteCmd(_WriteMultipleBlock, sector, 0x01);

		if (_response_no_err == res)
		{
			for (uint32_t cnt = 0; cnt < blkSize; cnt++)
			{
				res = SendData(txData, 512, 0xfc);

				if (_response_no_err == res)
				{
					txData += 512;
				}
				else
				{
					break;
				}
			}

			res = SendData(0, 512, 0xfd);
		}
	}

	UnselectCard();

	return res;
}


///Spi sd card read data
int SdCard::Read(uint8_t* rxData, uint32_t blkSize, uint32_t sector)
{
	uint8_t res = 0;

	if (SdCardType::_V2HC != sdcardType) sector = sector << 9;

	if (1 == blkSize)
	{
		res = WriteCmd(_ReadSingleBlock, sector, 0x01);

		if (_response_no_err == res)
		{
			res = RecvData(rxData, 512);
		}
	}
	else
	{
		res = WriteCmd(_ReadMultipleBlock, sector, 0x01);

		if (_response_no_err == res)
		{
			for (uint32_t cnt = 0; cnt < blkSize; cnt++)
			{
				res = RecvData(rxData, 512);

				if (_response_no_err == res)
				{
					rxData += 512;
				}
				else
				{
					break;
				}
			}
			WriteCmd(_StopTrans, 0, 0x01);
		}
	}

	UnselectCard();

	return res;
}


///Register driver
REGISTER_DRIVER(new SdCard(), DriverID::_block, sdcard);
