//###########################################################################
// Application3.cpp
// The overall framework of the application
//
// $Copyright: Copyright (C) village
//###########################################################################
#include "Kernel.h"
#include "Thread.h"
#include "HWManager.h"
#include "Application3.h"
#include "ff.h"

ThreadEndpoint* Application3::user;
ThreadHandlerCpp Application3::handler;


///Constructor
Application3::Application3()
{
}


///Initialize
void Application3::Initialize()
{
	gui.Initialize(&(HWManager::Instance()->ili9488));
	
	gui.disp.ShowString(0, 0, (uint8_t*)"hello vk.kernel");

	FATFS fs; DIR file_dir; FILINFO fileinfo;

	const TCHAR* path[] = { "0:", "1:" };

	uint16_t disp_y = 32;
	
	for (uint8_t i = 0; i < 2; i++)
	{
		gui.disp.ShowString(0, disp_y, (uint8_t*)"storage");
		gui.disp.ShowString(60, disp_y, (uint8_t*)path[i]);
		disp_y += 16;

		if (f_mount(&fs, path[i], 1) == FR_OK)
		{
			if (f_opendir(&file_dir, path[i]) == FR_OK)
			{
				while(1)
				{
					FRESULT res = f_readdir(&file_dir, &fileinfo);
					if (res != FR_OK || fileinfo.fname[0] == 0) break;
					
					gui.disp.ShowString(0, disp_y, (uint8_t*)fileinfo.fname);
					disp_y += 16;
				}
			}
			f_closedir(&file_dir);
			f_unmount(path[i]);
			disp_y += 16;
		}
	}

	user = this;
	handler = (ThreadHandlerCpp)(&Application3::Task);
	Thread::CreateTask(Application3::TaskHandler);
}


///Execute
void Application3::Execute()
{
	HWManager::Instance()->uartSerial.SendBytes((uint8_t*)"hello vk.kernel\r\n", 18);
	Thread::Sleep(1000);
}


///Task
void Application3::Task()
{
	UartSerial* serial = &(HWManager::Instance()->uartSerial);

	while (1)
	{
		serial->Execute();
		
		while (serial->HasBytes())
		{
			uint8_t byte = serial->ReadByte();
			serial->SendBytes(&byte, 1, false);
		}

		Thread::Sleep(100);
	}
}


///TaskHandler
void Application3::TaskHandler()
{
	if (user != 0) { (user->*handler)(); }
}


///Register module
REGISTER_MODULE(new Application3(), FUNCTION_ID(2), app_3);
