//###########################################################################
// FAT.cpp
// Definitions of the functions that manage fat file system
//
// $Copyright: Copyright (C) village
//###########################################################################
#include "FileSystem.h"
#include "FatSystem.h"
#include "Kernel.h"
#include "Debug.h"


/// @brief Constructor
FAT::FAT()
	:disk(NULL),
	dbr(NULL),
	startSector(0)
{
}


/// @brief Deconstructor
FAT::~FAT()
{
}


/// @brief Read DBR
int FAT::ReadDBR()
{
	static const uint8_t dbr_sector = 0;

	dbr = new DBR();

	if (NULL != dbr)
	{
		ReadSector((char*)dbr, 1, dbr_sector);
		
		if (magic == dbr->magic) return _OK;
	}

	return _ERR;
}


/// @brief 
/// @return 
int FAT::CheckFS()
{
	fat = new FATData();

	if (NULL != fat)
	{
		//Calc fat size
		if (0 != dbr->bpb.FATSz16)
			fat->FATSz = dbr->bpb.FATSz16;
		else
			fat->FATSz = dbr->fat32.FATSz32;
		
		//Calc total sectors
		if (0 != dbr->bpb.totSec16)
			fat->totSec = dbr->bpb.totSec16;
		else
			fat->totSec = dbr->bpb.totSec32;

		//Calc fat12/16 root dir sector
		fat->firstRootDirSecNum = dbr->bpb.rsvdSecCnt + (dbr->bpb.numFATs * fat->FATSz);
		fat->rootDirSectors = ((dbr->bpb.rootEntCnt * dir_entry_size) + (dbr->bpb.bytsPerSec - 1)) / dbr->bpb.bytsPerSec;
		
		//Calc fat data sector
		fat->firstDataSector = dbr->bpb.rsvdSecCnt + (dbr->bpb.numFATs * fat->FATSz) + fat->rootDirSectors;
		fat->dataSec = fat->totSec - (dbr->bpb.rsvdSecCnt + (dbr->bpb.numFATs * fat->FATSz) - fat->rootDirSectors);

		//Calc counts of clusters
		fat->countOfClusters = fat->dataSec / dbr->bpb.secPerClus;

		//Detected fat type
		if (fat->countOfClusters < 4085)
			fat->type = _FAT12;
		else if (fat->countOfClusters < 65525)
			fat->type = _FAT16;
		else
			fat->type = _FAT32;

		//fat32 root cluster
		fat->rootClus = (_FAT32 == fat->type) ? dbr->fat32.rootClus : 0;

		return _OK;
	}

	return _ERR;
}


/// @brief FAT mount
/// @return 
int FAT::Mount(const char* path, const char* mount, int opt, int fstSecNum)
{
	startSector = fstSecNum;

	disk = device.GetDriver(DriverID::_storage + 1);
	
	if (NULL == disk)
	{
		debug.Error("Not disk driver found");
		return _ERR;
	}

	if (_ERR == ReadDBR())
	{
		debug.Error("Not DBR found");
		return _ERR;
	}

	if (_ERR == CheckFS())
	{
		debug.Error("Not filesystem found");
		return _ERR;
	}

	debug.Output(Debug::_Lv2, "%s -> %s mount successful", path, mount);
	return _OK;
}


/// @brief FAT unmount
/// @return 
int FAT::Unmount(const char* mount)
{
	return _OK;
}


/// @brief FAT open
/// @param name 
/// @param mode 
/// @return 
int FAT::Open(const char* name, int mode)
{
	FATSDir* dir = SearchDir(name);
	if (NULL != dir)
	{
		return files.Add(dir);
	}
	return -1;
}


/// @brief FAT write
/// @param data 
/// @param size 
/// @param offset 
/// @return 
int FAT::Write(int fd, char* data, int size, int offset)
{
	return 0;
}


/// @brief FAT read
/// @param data 
/// @param size 
/// @param offset 
/// @return 
int FAT::Read(int fd, char* data, int size, int offset)
{
	FATSDir* dir = files.GetItem(fd);
	return ReadFile(data, size, dir);
}


/// @brief FAT seek
/// @param offset 
/// @return 
int FAT::Seek(int fd, int offset)
{
	return 0;
}


/// @brief FAT rename
/// @param old 
/// @param now 
/// @return 
int FAT::Rename(int fd, const char* old, const char* now)
{
	return 0;
}


/// @brief FAT copy
/// @param from 
/// @param to 
/// @return 
int FAT::Copy(int fd, const char* from, const char* to)
{
	return 0;
}


/// @brief FAT remove
/// @return 
int FAT::Remove(int fd)
{
	return 0;
}


/// @brief FAT size
/// @return 
int FAT::Size(int fd)
{
	FATSDir* dir = files.GetItem(fd);
	return FileSize(dir);
}


/// @brief FAT close
/// @return 
int FAT::Close(int fd)
{
	return 0;
}


///Register file system
REGISTER_FS(new FAT(), fat);
