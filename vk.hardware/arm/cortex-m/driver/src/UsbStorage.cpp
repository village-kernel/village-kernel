//###########################################################################
// UsbStorage.cpp
// Definitions of the functions that manage usb storage
//
// $Copyright: Copyright (C) village
//###########################################################################
#include "UsbStorage.h"
#include "usbd_desc.h"
#include "usbd_msc.h"
#include "Device.h"
#include "System.h"
#include "Kernel.h"


///USB storage macro
#define STORAGE_LUN_NBR                  2


//Static members
static Driver* storage[STORAGE_LUN_NBR] = { NULL };


///Override HAL_InitTick()
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
	return HAL_OK;
}


///Override HAL_Delay()
void HAL_Delay(uint32_t Delay)
{
	kernel->system->DelayMs(Delay);
}


///USB storage init
static int8_t STORAGE_Init_FS(uint8_t lun)
{
	storage[0] = kernel->device->GetDriver(DriverID::_storage + 0);
	storage[1] = kernel->device->GetDriver(DriverID::_storage + 1);
	return (USBD_OK);
}


///USB storage get capacity
static int8_t STORAGE_GetCapacity_FS(uint8_t lun, uint32_t* block_num, uint16_t* block_size)
{
	storage[lun]->IOCtrl(1, (void*)block_num);
	storage[lun]->IOCtrl(2, (void*)block_size);
	return (USBD_OK);
}


///USB storage is ready
static int8_t STORAGE_IsReady_FS(uint8_t lun)
{
	return (USBD_OK);
}


///USB storage is write protected
static int8_t STORAGE_IsWriteProtected_FS(uint8_t lun)
{
	return (USBD_OK);
}


///USB storage read
static int8_t STORAGE_Read_FS(uint8_t lun, uint8_t* buf, uint32_t blk_addr, uint16_t blk_len)
{
	storage[lun]->Read(buf, blk_len, blk_addr);
	return (USBD_OK);
}


///USB storage write
static int8_t STORAGE_Write_FS(uint8_t lun, uint8_t* buf, uint32_t blk_addr, uint16_t blk_len)
{
	storage[lun]->Write(buf, blk_len, blk_addr);
	return (USBD_OK);
}


///USB storage get max lun
static int8_t STORAGE_GetMaxLun_FS(void)
{
	return (STORAGE_LUN_NBR - 1);
}


///USB Mass storage Standard Inquiry Data
static const uint8_t STORAGE_Inquirydata[] =
{
	//LUN 0
	0x00,
	0x80,
	0x02,
	0x02,
	(STANDARD_INQUIRY_DATA_LEN - 5),
	0x00,
	0x00,
	0x00,
	'V', 'K', ' ', ' ', ' ', ' ', ' ', ' ', /* Manufacturer : 8 bytes */
	'v', 'i', 'l', 'l', 'a', 'g', 'e', ' ', /* Product      : 16 Bytes */
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
	'0', '.', '0' ,'1',                     /* Version      : 4 Bytes */
	//LUN 1
	0x00,
	0x80,
	0x02,
	0x02,
	(STANDARD_INQUIRY_DATA_LEN - 5),
	0x00,
	0x00,
	0x00,
	'V', 'K', ' ', ' ', ' ', ' ', ' ', ' ', /* Manufacturer : 8 bytes */
	'v', 'i', 'l', 'l', 'a', 'g', 'e', ' ', /* Product      : 16 Bytes */
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
	'0', '.', '0' ,'1'                      /* Version      : 4 Bytes */
};


///USB storage interface fops fs
static USBD_StorageTypeDef USBD_Storage_Interface_fops_FS =
{
	STORAGE_Init_FS,
	STORAGE_GetCapacity_FS,
	STORAGE_IsReady_FS,
	STORAGE_IsWriteProtected_FS,
	STORAGE_Read_FS,
	STORAGE_Write_FS,
	STORAGE_GetMaxLun_FS,
	(int8_t*)STORAGE_Inquirydata
};


///USB Device Core handle declaration
static USBD_HandleTypeDef hUsbDeviceFS;


///USB storage device initialize
bool UsbStorage::Open()
{
#ifdef STM32H7xx
	//Select usb clock
	RCC->D2CCIP2R |= RCC_D2CCIP2R_USBSEL;

	//Enable USB Voltage detector
	PWR->CR3 |= PWR_CR3_USB33DEN;
#endif
	//Initialize usb device
	USBD_Init(&hUsbDeviceFS, &FS_Desc, DEVICE_FS);

	//Register usb class
	USBD_RegisterClass(&hUsbDeviceFS, &USBD_MSC);

	//Register usb storage
	USBD_MSC_RegisterStorage(&hUsbDeviceFS, &USBD_Storage_Interface_fops_FS);

	//Start usbd
	USBD_Start(&hUsbDeviceFS);

	return true;
}


/// @brief Close
void UsbStorage::Close()
{

}


///Start mount usb storage to pc
void UsbStorage::Mount()
{
	USBD_Start(&hUsbDeviceFS);
}


///Stop mount usb storage to pc
void UsbStorage::Unmount()
{
	USBD_Stop(&hUsbDeviceFS);
}


///Register driver
REGISTER_DRIVER(new UsbStorage(), DriverID::_miscellaneous, usbStorage);
