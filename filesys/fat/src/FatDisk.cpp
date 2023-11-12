//###########################################################################
// FATDisk.cpp
// Definitions of the functions that manage fat file system
//
// $Copyright: Copyright (C) village
//###########################################################################
#include "FatDisk.h"
#include "Kernel.h"
#include "Debug.h"


/// @brief Setup
/// @param diskdrv 
/// @param fstSec 
/// @param dbr 
/// @param info 
void FatDisk::Setup(Driver* diskdrv, uint32_t fstSec, Info* info)
{
	this->diskdrv = diskdrv;
	this->fstSec  = fstSec;
	this->info    = info; 
}



/// @brief Get first clust
/// @param sfe 
/// @return clust
uint32_t FatDisk::GetFirstClust(ShortEntry sfe)
{
	return (uint32_t)(sfe.fstClustHI << 16) | sfe.fstClustLO;
}


/// @brief Cluster to sector number
/// @param clust 
/// @return sector number
uint32_t FatDisk::ClusterToSector(uint32_t clust)
{
	return ((clust - 2) * info->secPerClust) + info->firstDataSector;
}


/// @brief Get the next cluster
/// @param clust 
/// @return next cluster
uint32_t FatDisk::GetNextCluster(uint32_t clust)
{
	bool isEOC = false;
	uint32_t fatOffset = 0;
	uint32_t fatMaxOffset = 0;
	uint32_t fatClust = 0;

	if (_FAT16 == info->fatType)
	{
		fatOffset = clust * 2;
		fatMaxOffset = info->bytesPerSec / 2;
	}	
	else if (_FAT32 == info->fatType)
	{
		fatOffset = clust * 4;
		fatMaxOffset = info->bytesPerSec / 4;
	}

	uint32_t thisFatSecNum = info->rsvdSecCnt + (fatOffset / info->bytesPerSec);
	uint32_t thisFatEntOffset = clust % fatMaxOffset;

	char* secBuff = new char[info->bytesPerSec]();
	
	ReadOneSector(secBuff, thisFatSecNum);

	if (_FAT16 == info->fatType)
	{
		fatClust = ((uint16_t*)secBuff)[thisFatEntOffset];
		if (fatClust >= fat16_eoc_flag) isEOC = true;
	}
	else if (_FAT32 == info->fatType)
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
uint32_t FatDisk::SetNextCluster(uint32_t clust)
{
	uint32_t fatOffset = 0;
	uint32_t fatMaxOffset = 0;
	uint32_t fatClust = (clust < 2) ? 2 : clust;
	bool isAllocMode = (clust < 2) ? true : false;

	if (_FAT16 == info->fatType)
	{
		fatOffset = fatClust * 2;
		fatMaxOffset = info->bytesPerSec / 2;
	}	
	else if (_FAT32 == info->fatType)
	{
		fatOffset = fatClust * 4;
		fatMaxOffset = info->bytesPerSec / 4;
	}

	uint32_t thisFatSecNum = info->rsvdSecCnt + (fatOffset / info->bytesPerSec);
	uint32_t thisFatEntOffset = fatClust % fatMaxOffset;
	uint32_t nextFatSecNum = thisFatSecNum;
	uint32_t nextFatEntOffset = thisFatEntOffset;

	char* secBuff = new char[info->bytesPerSec]();

	//Read sector buff
	ReadOneSector(secBuff, thisFatSecNum);

	//Search next fat clust
	do
	{
		if (++nextFatEntOffset >= fatMaxOffset)
		{
			nextFatEntOffset = 0;

			if (nextFatSecNum <= info->endedOfFatSector)
			{
				ReadOneSector(secBuff, nextFatSecNum++);
			}
			else
			{
				delete[] secBuff;
				return 0;
			}
		}
		
		if (_FAT16 == info->fatType)
			clust = ((uint16_t*)secBuff)[nextFatEntOffset];
		else if (_FAT32 == info->fatType)
			clust = ((uint32_t*)secBuff)[nextFatEntOffset] & 0x0fffffff;

		fatClust++;
	}
	while (0 != clust);

	//Set fat clust list table
	if (_FAT16 == info->fatType)
	{
		if (nextFatSecNum == thisFatSecNum)
		{
			if (false == isAllocMode)
			{
				((uint16_t*)secBuff)[thisFatEntOffset] = fatClust;
			}
			
			((uint16_t*)secBuff)[nextFatEntOffset] = fat16_eoc_flag;
			WriteOneSector(secBuff, nextFatSecNum);
		}
		else
		{
			((uint16_t*)secBuff)[nextFatEntOffset] = fat16_eoc_flag;
			WriteOneSector(secBuff, nextFatSecNum);

			if (false == isAllocMode)
			{
				ReadOneSector(secBuff, thisFatSecNum);
				((uint16_t*)secBuff)[thisFatEntOffset] = fatClust;
				WriteOneSector(secBuff, thisFatSecNum);
			}
		}
	}
	else if (_FAT32 == info->fatType)
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
			WriteOneSector(secBuff, nextFatSecNum);
		}
		else
		{
			((uint32_t*)secBuff)[nextFatEntOffset] &= 0xf0000000;
			((uint32_t*)secBuff)[nextFatEntOffset] |= fat32_eoc_flag;
			WriteOneSector(secBuff, nextFatSecNum);
			
			if (false == isAllocMode)
			{
				ReadOneSector(secBuff, thisFatSecNum);
				((uint32_t*)secBuff)[thisFatEntOffset] &= 0xf0000000;
				((uint32_t*)secBuff)[thisFatEntOffset] |= fatClust;
				WriteOneSector(secBuff, thisFatSecNum);
			}
		}
	}

	delete[] secBuff;

	return fatClust;
}


/// @brief Clear prev cluster
/// @param clust 
/// @return 
uint32_t FatDisk::ClearPrevCluster(uint32_t clust)
{
	uint32_t fatOffset = 0;
	uint32_t fatMaxOffset = 0;
	uint32_t fatClust = clust;

	if (_FAT16 == info->fatType)
	{
		fatOffset = clust * 2;
		fatMaxOffset = info->bytesPerSec / 2;
	}	
	else if (_FAT32 == info->fatType)
	{
		fatOffset = clust * 4;
		fatMaxOffset = info->bytesPerSec / 4;
	}

	uint32_t thisFatSecNum = info->rsvdSecCnt + (fatOffset / info->bytesPerSec);
	uint32_t thisFatEntOffset = clust % fatMaxOffset;
	uint32_t prevFatSecNum = thisFatSecNum;
	uint32_t prevFatEntOffset = thisFatEntOffset;

	char* secBuff = new char[info->bytesPerSec]();

	//Read sector buff
	ReadOneSector(secBuff, thisFatSecNum);

	//Search prev fat clust
	do
	{
		if (--prevFatEntOffset <= 0)
		{
			prevFatEntOffset = fatMaxOffset;

			if (prevFatSecNum >= info->startOfFatSector)
			{
				ReadOneSector(secBuff, prevFatSecNum--);
			}
			else
			{
				delete[] secBuff;
				return 0;
			}
		}
		
		if (_FAT16 == info->fatType)
			fatClust = ((uint16_t*)secBuff)[prevFatEntOffset];
		else if (_FAT32 == info->fatType)
			fatClust = (((uint32_t*)secBuff)[prevFatEntOffset]) & 0x0fffffff;
	}
	while (fatClust != clust);

	//Set fat clust list table
	if (_FAT16 == info->fatType)
	{
		if (prevFatSecNum == thisFatSecNum)
		{
			((uint16_t*)secBuff)[thisFatEntOffset] = 0;
			((uint16_t*)secBuff)[prevFatEntOffset] = fat16_eoc_flag;
			WriteOneSector(secBuff, prevFatSecNum);
		}
		else
		{
			((uint16_t*)secBuff)[prevFatEntOffset] = fat16_eoc_flag;
			WriteOneSector(secBuff, prevFatSecNum);

			ReadOneSector(secBuff, thisFatSecNum);
			((uint16_t*)secBuff)[thisFatEntOffset] = 0;
			WriteOneSector(secBuff, thisFatSecNum);
		}
	}
	else if (_FAT32 == info->fatType)
	{
		if (prevFatSecNum == thisFatSecNum)
		{
			((uint32_t*)secBuff)[thisFatEntOffset] &= 0xf0000000;
			((uint32_t*)secBuff)[thisFatEntOffset] |= 0;

			((uint32_t*)secBuff)[prevFatEntOffset] &= 0xf0000000;
			((uint32_t*)secBuff)[prevFatEntOffset] |= fat32_eoc_flag;
			WriteOneSector(secBuff, prevFatSecNum);
		}
		else
		{
			((uint32_t*)secBuff)[prevFatEntOffset] &= 0xf0000000;
			((uint32_t*)secBuff)[prevFatEntOffset] |= fat32_eoc_flag;
			WriteOneSector(secBuff, prevFatSecNum);
			
			ReadOneSector(secBuff, thisFatSecNum);
			((uint32_t*)secBuff)[thisFatEntOffset] &= 0xf0000000;
			((uint32_t*)secBuff)[thisFatEntOffset] |= 0;
			WriteOneSector(secBuff, thisFatSecNum);
		}
	}

	delete[] secBuff;

	return fatClust;
}


/// @brief Calc first sector
/// @param clust 
/// @param sector 
void FatDisk::CalcFirstSector(DirEntry* dirent)
{
	if (GetFirstClust(dirent->body.sfe) < 2)
	{
		if (_FAT16 == info->fatType)
		{
			dirent->temp.clust  = 0;
			dirent->temp.sector = info->firstRootSector;
		}
		else if (_FAT32 == info->fatType)
		{
			dirent->temp.clust  = info->rootClust;
			dirent->temp.sector = ClusterToSector(dirent->temp.clust);
		}
	}
	else
	{
		dirent->temp.clust  = GetFirstClust(dirent->body.sfe);
		dirent->temp.sector = ClusterToSector(dirent->temp.clust);
	}
}


/// @brief Calc next sector
/// @param clust 
/// @param sector 
void FatDisk::CalcNextSector(DirEntry* dirent)
{
	//FAT16 root dir
	if (dirent->temp.clust < 2)
	{
		uint32_t dirEndedSec = info->firstRootSector + info->countOfRootSecs;
		dirent->temp.sector   = (++dirent->temp.sector < dirEndedSec) ? dirent->temp.sector : 0;
	}
	//FatDisk data dir
	else
	{ 
		if ((++dirent->temp.sector - ClusterToSector(dirent->temp.clust)) >= info->secPerClust)
		{
			dirent->temp.clust = GetNextCluster(dirent->temp.clust);
			dirent->temp.sector = (0 != dirent->temp.clust) ? ClusterToSector(dirent->temp.clust) : 0;
		}
	}
}


/// @brief 
/// @param clust 
/// @param sector 
void FatDisk::CalcPrevSector(DirEntry* dirent)
{
	//FAT16 root dir
	if (dirent->temp.clust < 2)
	{
		uint32_t dirStartSec = info->firstRootSector;
		dirent->temp.sector = (--dirent->temp.sector >= dirStartSec) ? dirent->temp.sector : 0;
	}
	//FatDisk data dir
	else
	{ 
		if ((--dirent->temp.sector - ClusterToSector(dirent->temp.clust)) >= info->secPerClust)
		{
			dirent->temp.clust = GetPrevCluster(dirent->temp.clust);
			dirent->temp.sector = (0 != dirent->temp.clust) ? ClusterToSector(dirent->temp.clust) : 0;
		}
	}
}


/// @brief 
/// @param dirent 
void FatDisk::ReadUnionEntries(DirEntry* dirent)
{
	ReadOneSector((char*)dirent->temp.unients, dirent->temp.sector);
}


/// @brief 
/// @param dirent 
void FatDisk::WriteUnionEntries(DirEntry* dirent)
{
	WriteOneSector((char*)dirent->temp.unients, dirent->temp.sector);
}


/// @brief Read one sector
/// @param data 
/// @param secSize 
/// @param sector 
/// @return 
uint32_t FatDisk::ReadOneSector(char* data, uint32_t sector)
{
	if (NULL != diskdrv)
	{
		diskdrv->Read((uint8_t*)data, 1, sector + fstSec);
	}
	return 1;
}


/// @brief Read Sector
/// @param data 
/// @param secSize 
/// @param sector 
/// @return read sector size
uint32_t FatDisk::ReadSector(char* data, uint32_t secSize, uint32_t sector)
{
	if (NULL != diskdrv)
	{
		diskdrv->Read((uint8_t*)data, secSize, sector + fstSec);
	}
	return secSize;
}


/// @brief Read cluster
/// @param data 
/// @param clusSize 
/// @param clust 
/// @return read cluster size
uint32_t FatDisk::ReadCluster(char* data, uint32_t clustSize, uint32_t clust)
{
	uint32_t bytesPerSec = info->bytesPerSec;
	uint32_t secPerClust = info->secPerClust;

	for (uint32_t i = 0; i < clustSize; i++)
	{
		uint32_t sector = ClusterToSector(clust);
		uint32_t offset = i * bytesPerSec * secPerClust;

		if (secPerClust != ReadSector(data + offset, secPerClust, sector))
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

 
/// @brief Write one sector
/// @param data 
/// @param sector 
/// @return 
uint32_t FatDisk::WriteOneSector(char* data, uint32_t sector)
{
	if (NULL != diskdrv)
	{
		diskdrv->Write((uint8_t*)data, 1, sector + fstSec);
	}
	return 1;
}


/// @brief Write sector
/// @param data 
/// @param secSize 
/// @param sector 
/// @return 
uint32_t FatDisk::WriteSector(char* data, uint32_t secSize, uint32_t sector)
{
	if (NULL != diskdrv)
	{
		diskdrv->Write((uint8_t*)data, secSize, sector + fstSec);
	}
	return secSize;
}


/// @brief Write cluster
/// @param data 
/// @param clustSize 
/// @param clust 
/// @return 
uint32_t FatDisk::WriteCluster(char* data, uint32_t clustSize, uint32_t clust)
{
	uint32_t bytesPerSec = info->bytesPerSec;
	uint32_t secPerClust = info->secPerClust;

	for (uint32_t i = 0; i < clustSize; i++)
	{
		uint32_t sector = ClusterToSector(clust);
		uint32_t offset = i * bytesPerSec * secPerClust;

		if (secPerClust != WriteSector(data + offset, secPerClust, sector))
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
uint32_t FatDisk::ClearCluster(uint32_t clust, uint32_t clustSize)
{
	uint32_t bytesPerSec = info->bytesPerSec;
	uint32_t secPerClust = info->secPerClust;

	char* zero = new char[bytesPerSec * secPerClust]();
	memset(zero, 0, bytesPerSec * secPerClust);

	for (uint32_t i = 0; i < clustSize; i++)
	{
		uint32_t sector = ClusterToSector(clust);

		if (secPerClust != WriteSector(zero, secPerClust, sector))
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
uint32_t FatDisk::AllocCluster(uint32_t clustSize)
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
uint32_t FatDisk::FreeCluster(uint32_t clustSize, uint32_t clust)
{
	for (uint32_t i = 0; i < clustSize; i++)
	{
		clust = ClearPrevCluster(clust);
		if (0 == clust) return 0;
	}
	return clust;
}
