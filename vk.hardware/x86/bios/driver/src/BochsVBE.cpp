//###########################################################################
// BochsVBE.cpp
// Definitions of the functions that manage Bochs VBE
//
// $Copyright: Copyright (C) village
//###########################################################################
#include "Hardware.h"
#include "Kernel.h"
#include "Driver.h"
#include "LcdDriver.h"

#include "PCIController.cpp"

#define VBE_DISPI_IOPORT_INDEX       (0x01CE)
#define VBE_DISPI_IOPORT_DATA        (0x01CF)

#define VBE_DISPI_INDEX_ID           (0)
#define VBE_DISPI_INDEX_XRES         (1)
#define VBE_DISPI_INDEX_YRES         (2)
#define VBE_DISPI_INDEX_BPP          (3)
#define VBE_DISPI_INDEX_ENABLE       (4)
#define VBE_DISPI_INDEX_BANK         (5)
#define VBE_DISPI_INDEX_VIRT_WIDTH   (6)
#define VBE_DISPI_INDEX_VIRT_HEIGHT  (7)
#define VBE_DISPI_INDEX_X_OFFSET     (8)
#define VBE_DISPI_INDEX_Y_OFFSET     (9)

#define VBE_DISPI_DISABLED           (0x00)
#define VBE_DISPI_ENABLED            (0x01)
#define VBE_DISPI_LFB_ENABLED        (0x40)
#define VBE_DISPI_NOCLEARMEM         (0x80)

#define VBE_DISPI_BPP_4              (0x04)
#define VBE_DISPI_BPP_8              (0x08)
#define VBE_DISPI_BPP_15             (0x0F)
#define VBE_DISPI_BPP_16             (0x10)
#define VBE_DISPI_BPP_24             (0x18)
#define VBE_DISPI_BPP_32             (0x20)

#define VBE_DISPI_ID0                (0xB0C0)
#define VBE_DISPI_ID1                (0xB0C1)
#define VBE_DISPI_ID2                (0xB0C2)
#define VBE_DISPI_ID3                (0xB0C3)
#define VBE_DISPI_ID4                (0xB0C4)
#define VBE_DISPI_ID5                (0xB0C5)


/// @brief BochsVBE
class BochsVBE : public LcdDriver
{
private:
	//Members
	uint16_t*     vmap;
	PCIController pci;
private:
	/// @brief BochsVBE write data
	/// @param reg 
	/// @param value 
	void WriteData(uint32_t reg, uint16_t value)
	{
		uint16_t* videos = (uint16_t*)vmap;
		videos[reg] = value;
	}


	/// @brief BochsVBE read data
	/// @param reg 
	/// @return 
	uint16_t ReadData(uint32_t reg)
	{
		uint16_t* videos = (uint16_t*)vmap;
		return videos[reg];
	}


	/// @brief BochsVBE write reg
	/// @param reg 
	/// @param value 
	void WriteReg(uint16_t reg, uint16_t value)
	{
		PortWordOut(VBE_DISPI_IOPORT_INDEX, reg);
		PortWordOut(VBE_DISPI_IOPORT_DATA, value);
	}


	/// @brief BochsVBE read reg
	/// @param reg 
	/// @return 
	uint16_t ReadReg(uint16_t reg)
	{
		PortWordOut(VBE_DISPI_IOPORT_INDEX, reg);
		return PortWordIn(VBE_DISPI_IOPORT_DATA);
	}


	/// @brief Check is available
	/// @return 
	bool IsBochsVBEAvailable()
	{
		return (ReadReg(VBE_DISPI_INDEX_ID) >= VBE_DISPI_ID4);
	}


	/// @brief Set video mode
	/// @param width 
	/// @param height 
	/// @param bitdepth 
	/// @param isUseLinearFrameBuffer 
	/// @param isClearVideoMemory 
	void SetVideoMode(uint16_t width, uint16_t height, uint16_t bitdepth, bool isUseLinearFrameBuffer, bool isClearVideoMemory)
	{
		WriteReg(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
		WriteReg(VBE_DISPI_INDEX_XRES, width);
		WriteReg(VBE_DISPI_INDEX_YRES, height);
		WriteReg(VBE_DISPI_INDEX_BPP,  bitdepth);
		WriteReg(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED |
				(isUseLinearFrameBuffer ? VBE_DISPI_LFB_ENABLED : 0) |
				(isClearVideoMemory ? 0 : VBE_DISPI_NOCLEARMEM));
	}


	/// @brief Set bank
	/// @param bankNumber 
	void SetBank(uint16_t bankNumber)
	{
		WriteReg(VBE_DISPI_INDEX_BANK, bankNumber);
	}
public:
	/// @brief Constructor
	BochsVBE()
		:vmap((uint16_t*)0xA00000)
	{
	}


	/// @brief Destructor
	~BochsVBE()
	{
	}


	/// @brief BochsVBE initialize
	void Initialize()
	{
		device.width    = 1024;
		device.height   = 768;
		device.bitdepth = 0x10; // 16 bpp

		if (IsBochsVBEAvailable())
		{
			//Set video mode
			SetVideoMode(device.width, device.height, device.bitdepth, true, true);
			
			//Get PCI device 0x1234:0x1111 BAR 0
			vmap = (uint16_t*)pci.ReadBAR(0x1234, 0x1111, 0);
		}
	}


	/// @brief BochsVBE draw point
	/// @param x 
	/// @param y 
	/// @param color 
	void DrawPoint(uint16_t x, uint16_t y, uint16_t color)
	{
		uint32_t reg = (uint32_t)x + (uint32_t)y * device.width;
		WriteData(reg, color);
	}


	/// @brief BochsVBE read point
	/// @param x 
	/// @param y 
	/// @return 
	uint16_t ReadPoint(uint16_t x, uint16_t y)
	{
		uint32_t reg = (uint32_t)x + (uint32_t)y * device.width;
		return ReadData(reg);
	}


	/// @brief BochsVBE clear
	/// @param color 
	void Clear(uint16_t color = 0)
	{
		for (uint16_t y = 0; y < device.height; y++)
		{
			for (uint16_t x = 0; x < device.width; x++)
			{
				DrawPoint(x, y, color);
			}
		}
	}
};


/// @brief BochsVBEDrv
class BochsVBEDrv : public Driver
{
private:
	//Members
	BochsVBE* bochsVBE;
public:
	/// @brief Constructor
	BochsVBEDrv()
	{
		bochsVBE = new BochsVBE;
	}


	/// @brief Destructor
	~BochsVBEDrv()
	{
		delete bochsVBE;
	}


	/// @brief 
	void Initialize()
	{		
		bochsVBE->Initialize();
	}


	/// @brief IOCtrl
	/// @param cmd 
	/// @param data 
	/// @return 
	int IOCtrl(uint8_t cmd, void* data)
	{
		LcdDriver** lcd = (LcdDriver**)data;

		*lcd = bochsVBE;

		return 0;
	}
};


///Register driver
REGISTER_DRIVER(new BochsVBEDrv(), DriverID::_display, display);