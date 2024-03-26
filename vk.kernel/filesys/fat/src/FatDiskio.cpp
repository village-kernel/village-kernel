//###########################################################################
// FatDiskio.cpp
// Definitions of the functions that manage fat file system
//
// $Copyright: Copyright (C) village
//###########################################################################
#include "FatDiskio.h"
#include "Kernel.h"


/// @brief Setup
/// @param diskdrv 
/// @param fstSec 
bool FatDiskio::Setup(Driver* diskdrv, uint32_t fstSec)
{
	this->diskdrv = diskdrv;
	this->fstSec  = fstSec;

	if (!CheckFileSystem())
	{
		kernel->debug.Error("Not filesystem found");
		return false;
	}
	return true;
}


/// @brief Check file system
/// @return 
bool FatDiskio::CheckFileSystem()
{
	//Read dbr
	DBR* dbr = new DBR();
	ReadSector((char*)dbr, 0);

	//Check dbr
	if (magic != dbr->magic)
	{
		delete dbr;
		return false;
	}

	//Calc fat size
	if (0 != dbr->bpb.FATSz16)
		info.fatSize = dbr->bpb.FATSz16;
	else
		info.fatSize = dbr->fat32.FATSz32;
	
	//Calc total sectors
	if (0 != dbr->bpb.totSec16)
		info.totalSectors = dbr->bpb.totSec16;
	else
		info.totalSectors = dbr->bpb.totSec32;

	//Calc rsvd sector count
	info.rsvdSecCnt = dbr->bpb.rsvdSecCnt;

	//Calc the sector number of start/ended of FAT
	info.startOfFatSector = dbr->bpb.rsvdSecCnt;
	info.endedOfFatSector = dbr->bpb.rsvdSecCnt + (dbr->bpb.numFATs * info.fatSize) - 1;

	//Calc fat12/16 root dir sector
	info.firstRootSector = dbr->bpb.rsvdSecCnt + (dbr->bpb.numFATs * info.fatSize);
	info.countOfRootSecs = ((dbr->bpb.rootEntCnt * dir_entry_size) + (dbr->bpb.bytesPerSec - 1)) / dbr->bpb.bytesPerSec;
	
	//Calc fat data sector
	info.firstDataSector = dbr->bpb.rsvdSecCnt + (dbr->bpb.numFATs * info.fatSize) + info.countOfRootSecs;
	info.countOfDataSecs = info.totalSectors - (dbr->bpb.rsvdSecCnt + (dbr->bpb.numFATs * info.fatSize) - info.countOfRootSecs);

	//Calc counts of clusters
	info.countOfClusters = info.countOfDataSecs / dbr->bpb.secPerClust;

	//Detected fat type
	if (info.countOfClusters < 4085)
		info.fatType = _FAT12;
	else if (info.countOfClusters < 65525)
		info.fatType = _FAT16;
	else
		info.fatType = _FAT32;

	//Fat32 root cluster
	info.rootClust = (_FAT32 == info.fatType) ? dbr->fat32.rootClust : 0;

	//Calc the info data
	info.entriesPerSec = dbr->bpb.bytesPerSec / dir_entry_size;
	info.bytesPerSec = dbr->bpb.bytesPerSec;
	info.secPerClust = dbr->bpb.secPerClust;

	delete dbr;
	return true;
}


/// @brief Read Sector
/// @param data 
/// @param sector 
/// @param secSize 
/// @return read sector size
uint32_t FatDiskio::ReadSector(char* data, uint32_t sector, uint32_t secSize)
{
	if (NULL != diskdrv)
	{
		diskdrv->Read((uint8_t*)data, secSize, sector + fstSec);
	}
	return secSize;
}


/// @brief Write sector
/// @param data 
/// @param sector 
/// @param secSize 
/// @return 
uint32_t FatDiskio::WriteSector(char* data, uint32_t sector, uint32_t secSize)
{
	if (NULL != diskdrv)
	{
		diskdrv->Write((uint8_t*)data, secSize, sector + fstSec);
	}
	return secSize;
}


/// @brief Cluster to sector number
/// @param clust 
/// @return sector number
uint32_t FatDiskio::ClusterToSector(uint32_t clust)
{
	return ((clust - 2) * info.secPerClust) + info.firstDataSector;
}


/// @brief Get the next cluster
/// @param clust 
/// @return next cluster
uint32_t FatDiskio::GetNextCluster(uint32_t clust)
{
	bool isEOC = false;
	uint32_t fatOffset = 0;
	uint32_t fatMaxOffset = 0;
	uint32_t fatClust = 0;

	if (_FAT16 == info.fatType)
	{
		fatOffset = clust * 2;
		fatMaxOffset = info.bytesPerSec / 2;
	}	
	else if (_FAT32 == info.fatType)
	{
		fatOffset = clust * 4;
		fatMaxOffset = info.bytesPerSec / 4;
	}

	uint32_t thisFatSecNum = info.rsvdSecCnt + (fatOffset / info.bytesPerSec);
	uint32_t thisFatEntOffset = clust % fatMaxOffset;

	char* secBuff = new char[info.bytesPerSec]();
	
	ReadSector(secBuff, thisFatSecNum);

	if (_FAT16 == info.fatType)
	{
		fatClust = ((uint16_t*)secBuff)[thisFatEntOffset];
		if (fatClust >= fat16_eoc_flag) isEOC = true;
	}
	else if (_FAT32 == info.fatType)
	{
		fatClust = ((uint32_t*)secBuff)[thisFatEntOffset] & 0x0fffffff;
		if (fatClust >= fat32_eoc_flag) isEOC = true;
	}
	
	delete[] secBuff;

	return isEOC ? 0 : fatClust;
}


/// @brief Set the next cluster
/// @param clust 
/// @return next cluster
uint32_t FatDiskio::SetNextCluster(uint32_t clust)
{
	uint32_t fatOffset = 0;
	uint32_t fatMaxOffset = 0;
	uint32_t fatClust = (clust < 2) ? 2 : clust;
	bool isAllocMode = (clust < 2) ? true : false;

	if (_FAT16 == info.fatType)
	{
		fatOffset = fatClust * 2;
		fatMaxOffset = info.bytesPerSec / 2;
	}	
	else if (_FAT32 == info.fatType)
	{
		fatOffset = fatClust * 4;
		fatMaxOffset = info.bytesPerSec / 4;
	}

	uint32_t thisFatSecNum = info.rsvdSecCnt + (fatOffset / info.bytesPerSec);
	uint32_t thisFatEntOffset = fatClust % fatMaxOffset;
	uint32_t nextFatSecNum = thisFatSecNum;
	uint32_t nextFatEntOffset = thisFatEntOffset;

	char* secBuff = new char[info.bytesPerSec]();

	//Read sector buff
	ReadSector(secBuff, thisFatSecNum);

	//Search next fat clust
	do
	{
		if (++nextFatEntOffset >= fatMaxOffset)
		{
			nextFatEntOffset = 0;

			if (nextFatSecNum <= info.endedOfFatSector)
			{
				ReadSector(secBuff, nextFatSecNum++);
			}
			else
			{
				delete[] secBuff;
				return 0;
			}
		}
		
		if (_FAT16 == info.fatType)
			clust = ((uint16_t*)secBuff)[nextFatEntOffset];
		else if (_FAT32 == info.fatType)
			clust = ((uint32_t*)secBuff)[nextFatEntOffset] & 0x0fffffff;

		fatClust++;
	}
	while (0 != clust);

	//Set fat clust list table
	if (_FAT16 == info.fatType)
	{
		if (nextFatSecNum == thisFatSecNum)
		{
			if (false == isAllocMode)
			{
				((uint16_t*)secBuff)[thisFatEntOffset] = fatClust;
			}
			
			((uint16_t*)secBuff)[nextFatEntOffset] = fat16_eoc_flag;
			WriteSector(secBuff, nextFatSecNum);
		}
		else
		{
			((uint16_t*)secBuff)[nextFatEntOffset] = fat16_eoc_flag;
			WriteSector(secBuff, nextFatSecNum);

			if (false == isAllocMode)
			{
				ReadSector(secBuff, thisFatSecNum);
				((uint16_t*)secBuff)[thisFatEntOffset] = fatClust;
				WriteSector(secBuff, thisFatSecNum);
			}
		}
	}
	else if (_FAT32 == info.fatType)
	{
		if (nextFatSecNum == thisFatSecNum)
		{
			if (false == isAllocMode)
			{
				((uint32_t*)secBuff)[thisFatEntOffset] &= 0xf0000000;
				((uint32_t*)secBuff)[thisFatEntOffset] |= fatClust;
			}

			((uint32_t*)secBuff)[nextFatEntOffset] &= 0xf0000000;
			((uint32_t*)secBuff)[nextFatEntOffset] |= fat32_eoc_flag;
			WriteSector(secBuff, nextFatSecNum);
		}
		else
		{
			((uint32_t*)secBuff)[nextFatEntOffset] &= 0xf0000000;
			((uint32_t*)secBuff)[nextFatEntOffset] |= fat32_eoc_flag;
			WriteSector(secBuff, nextFatSecNum);
			
			if (false == isAllocMode)
			{
				ReadSector(secBuff, thisFatSecNum);
				((uint32_t*)secBuff)[thisFatEntOffset] &= 0xf0000000;
				((uint32_t*)secBuff)[thisFatEntOffset] |= fatClust;
				WriteSector(secBuff, thisFatSecNum);
			}
		}
	}

	delete[] secBuff;

	return fatClust;
}


/// @brief Clear prev cluster
/// @param clust 
/// @return 
uint32_t FatDiskio::ClearPrevCluster(uint32_t clust)
{
	uint32_t fatOffset = 0;
	uint32_t fatMaxOffset = 0;
	uint32_t fatClust = clust;

	if (_FAT16 == info.fatType)
	{
		fatOffset = clust * 2;
		fatMaxOffset = info.bytesPerSec / 2;
	}	
	else if (_FAT32 == info.fatType)
	{
		fatOffset = clust * 4;
		fatMaxOffset = info.bytesPerSec / 4;
	}

	uint32_t thisFatSecNum = info.rsvdSecCnt + (fatOffset / info.bytesPerSec);
	uint32_t thisFatEntOffset = clust % fatMaxOffset;
	uint32_t prevFatSecNum = thisFatSecNum;
	uint32_t prevFatEntOffset = thisFatEntOffset;

	char* secBuff = new char[info.bytesPerSec]();

	//Read sector buff
	ReadSector(secBuff, thisFatSecNum);

	//Search prev fat clust
	do
	{
		if (--prevFatEntOffset <= 0)
		{
			prevFatEntOffset = fatMaxOffset;

			if (prevFatSecNum >= info.startOfFatSector)
			{
				ReadSector(secBuff, prevFatSecNum--);
			}
			else
			{
				delete[] secBuff;
				return 0;
			}
		}
		
		if (_FAT16 == info.fatType)
			fatClust = ((uint16_t*)secBuff)[prevFatEntOffset];
		else if (_FAT32 == info.fatType)
			fatClust = (((uint32_t*)secBuff)[prevFatEntOffset]) & 0x0fffffff;
	}
	while (fatClust != clust);

	//Set fat clust list table
	if (_FAT16 == info.fatType)
	{
		if (prevFatSecNum == thisFatSecNum)
		{
			((uint16_t*)secBuff)[thisFatEntOffset] = 0;
			((uint16_t*)secBuff)[prevFatEntOffset] = fat16_eoc_flag;
			WriteSector(secBuff, prevFatSecNum);
		}
		else
		{
			((uint16_t*)secBuff)[prevFatEntOffset] = fat16_eoc_flag;
			WriteSector(secBuff, prevFatSecNum);

			ReadSector(secBuff, thisFatSecNum);
			((uint16_t*)secBuff)[thisFatEntOffset] = 0;
			WriteSector(secBuff, thisFatSecNum);
		}
	}
	else if (_FAT32 == info.fatType)
	{
		if (prevFatSecNum == thisFatSecNum)
		{
			((uint32_t*)secBuff)[thisFatEntOffset] &= 0xf0000000;
			((uint32_t*)secBuff)[thisFatEntOffset] |= 0;

			((uint32_t*)secBuff)[prevFatEntOffset] &= 0xf0000000;
			((uint32_t*)secBuff)[prevFatEntOffset] |= fat32_eoc_flag;
			WriteSector(secBuff, prevFatSecNum);
		}
		else
		{
			((uint32_t*)secBuff)[prevFatEntOffset] &= 0xf0000000;
			((uint32_t*)secBuff)[prevFatEntOffset] |= fat32_eoc_flag;
			WriteSector(secBuff, prevFatSecNum);
			
			ReadSector(secBuff, thisFatSecNum);
			((uint32_t*)secBuff)[thisFatEntOffset] &= 0xf0000000;
			((uint32_t*)secBuff)[thisFatEntOffset] |= 0;
			WriteSector(secBuff, thisFatSecNum);
		}
	}

	delete[] secBuff;

	return fatClust;
}


/// @brief Read cluster
/// @param data 
/// @param clusSize 
/// @param clust 
/// @return read cluster size
uint32_t FatDiskio::ReadCluster(char* data, uint32_t clust, uint32_t clustSize)
{
	uint32_t bytesPerSec = info.bytesPerSec;
	uint32_t secPerClust = info.secPerClust;

	for (uint32_t i = 0; i < clustSize; i++)
	{
		uint32_t sector = ClusterToSector(clust);
		uint32_t offset = i * bytesPerSec * secPerClust;

		if (secPerClust != ReadSector(data + offset, sector, secPerClust))
		{
			return i + 1;
		}

		if (clustSize > 1)
		{
			clust = GetNextCluster(clust);
			if (0 == clust) return i + 1;
		}
	}

	return clustSize;
}


/// @brief Write cluster
/// @param data 
/// @param clustSize 
/// @param clust 
/// @return 
uint32_t FatDiskio::WriteCluster(char* data, uint32_t clust, uint32_t clustSize)
{
	uint32_t bytesPerSec = info.bytesPerSec;
	uint32_t secPerClust = info.secPerClust;

	for (uint32_t i = 0; i < clustSize; i++)
	{
		uint32_t sector = ClusterToSector(clust);
		uint32_t offset = i * bytesPerSec * secPerClust;

		if (secPerClust != WriteSector(data + offset, sector, secPerClust))
		{
			return i + 1;
		}

		if (clustSize > 1)
		{
			clust = SetNextCluster(clust);
			if (0 == clust) return i + 1;
		}
	}

	return clustSize;
}


/// @brief Clear cluster
/// @param clustSize 
/// @param clust 
/// @return 
uint32_t FatDiskio::ClearCluster(uint32_t clust, uint32_t clustSize)
{
	uint32_t bytesPerSec = info.bytesPerSec;
	uint32_t secPerClust = info.secPerClust;

	char* zero = new char[bytesPerSec * secPerClust]();
	memset(zero, 0, bytesPerSec * secPerClust);

	for (uint32_t i = 0; i < clustSize; i++)
	{
		uint32_t sector = ClusterToSector(clust);

		if (secPerClust != WriteSector(zero, sector, secPerClust))
		{
			delete[] zero;
			return i + 1;
		}

		if (clustSize > 1)
		{
			clust = GetNextCluster(clust);
			if (0 == clust) { delete[] zero; return i + 1; }
		}
	}

	delete[] zero;
	return clustSize;
}


/// @brief Alloc cluster
/// @param clustSize 
/// @return clust
uint32_t FatDiskio::AllocCluster(uint32_t clustSize)
{
	uint32_t clust = 0;
	uint32_t fstClust = 0;
	for (uint32_t i = 0; i < clustSize; i++)
	{
		clust = SetNextCluster(clust);
		if (0 == fstClust) fstClust = clust;
		if (0 == clust) return 0;
		ClearCluster(clust, 1);
	}
	return fstClust;
}


/// @brief Free cluster
/// @param clustSize 
/// @param clust 
/// @return 
uint32_t FatDiskio::FreeCluster(uint32_t clustSize, uint32_t clust)
{
	for (uint32_t i = 0; i < clustSize; i++)
	{
		clust = ClearPrevCluster(clust);
		if (0 == clust) return 0;
	}
	return clust;
}


/// @brief Get info
/// @return 
FatDiskio::Info& FatDiskio::GetInfo()
{
	return info;
}
