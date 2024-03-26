//###########################################################################
// FAT.cpp
// Definitions of the functions that manage fat file system
//
// $Copyright: Copyright (C) village
//###########################################################################
#include "FileSystem.h"
#include "FatSystem.h"
#include "FatVolume.h"
#include "Kernel.h"
#include "stdio.h"


/// @brief Constructor
FatSystem::FatSystem()
{
}


/// @brief Destructor
FatSystem::~FatSystem()
{
}


/// @brief Setup
void FatSystem::Setup()
{
	FileSystem* filesys = (FileSystem*)kernel->feature.GetModule("fileSystem");
	if (NULL == filesys)
	{
		kernel->debug.Error("file system feature not support");
		return;
	}

	diskdrv = kernel->device.GetDriver("disk0");
	if (NULL == diskdrv || !diskdrv->Open())
	{
		kernel->debug.Error("Not disk driver found");
		return;
	}

	if (!ReadMBR())
	{
		kernel->debug.Error("Not a valid disk");
		return;
	}

	for (uint8_t i = 0; i < 4; i++)
	{
		if (CheckDPT(&mbr->dpt[i]))
		{
			FatVolume* fatOpts = new FatVolume();

			if (!fatOpts->Setup(diskdrv, mbr->dpt[i].relativeSectors))
			{
				delete fatOpts;
				continue;
			}

			filesys->RegisterOpts(fatOpts);
		}
	}

	kernel->debug.Output(Debug::_Lv2, "Initialize FAT file system successful");
}


/// @brief FAT exit
void FatSystem::Exit()
{
	delete mbr;
	diskdrv->Close();
}


/// @brief Read MBR
bool FatSystem::ReadMBR()
{
	static const uint8_t mbr_sector = 0;

	mbr = new MBR();

	if (NULL != mbr)
	{
		diskdrv->Read((uint8_t*)mbr, 1, mbr_sector);

		if (magic == mbr->magic) return true;
	}

	return false;
}


/// @brief 
/// @param dpt 
/// @return 
bool FatSystem::CheckDPT(DPT* dpt)
{
	return (dpt->systemID == 11);
}


///Register file system
REGISTER_FS(new FatSystem(), fat);
