//###########################################################################
// Fatfs.cpp
// Definitions of the functions that manage fat file system
//
// $Copyright: Copyright (C) village
//###########################################################################
#include "FileSystem.h"
#include "Kernel.h"
#include "Fatfs.h"
#include "Debug.h"


/// @brief Constructor
Fatfs::Fatfs()
	:disk(NULL),
	mbr(NULL),
	dbr(NULL)
{
}


/// @brief Deconstructor
Fatfs::~Fatfs()
{
}


/// @brief CHS to LBA address
/// @param path 
/// @param mount 
/// @param opt 
/// @return 
uint32_t Fatfs::CHS2LBA(uint8_t head, uint8_t sector, uint16_t cylinder)
{
	//LBA = (C × HPC + H) × SPT + (S − 1)
	return (cylinder * 16 + head) * 63 + (sector - 1);
}


/// @brief 
/// @param lba 
/// @param head 
/// @param sector 
/// @param cylinder 
void Fatfs::LBA2CHS(uint32_t lba, uint8_t& head, uint8_t& sector, uint16_t& cylinder)
{
	//C = LBA ÷ (HPC × SPT)
	//H = (LBA ÷ SPT) mod HPC
	//S = (LBA mod SPT) + 1
}


/// @brief 
/// @param fcbName 
/// @return 
uint8_t Fatfs::ChkSum(char* fcbName)
{
	uint8_t sum = 0;

	for (int16_t fcbNameLen = 11; fcbNameLen != 0; fcbNameLen--)
	{
		sum = ((sum & 1) ? 0x80 : 0) + (sum >> 1) + *fcbName++;
	}

	return sum;
}


/// @brief Read MBR
bool Fatfs::ReadMBR()
{
	static const uint8_t mbr_sector = 0;
	
	mbr = new MBR();

	if (NULL != mbr)
	{
		disk->Read((uint8_t*)mbr, 1, mbr_sector);
		
		if (magic == mbr->magic) return true;
	}

	return false;
}


/// @brief Read DBR
bool Fatfs::ReadDBR()
{
	dbr = new DBR();

	if (NULL != dbr)
	{
		disk->Read((uint8_t*)dbr, 1, mbr->dpt[0].relativeSectors);
		
		if (magic == dbr->magic) return true;
	}

	return false;
}


/// @brief 
/// @return 
bool Fatfs::CheckFS()
{
	fat = new FATData();

	if (NULL != fat)
	{
		if (0 != dbr->bpb.FATSz16)
			fat->FATSz = dbr->bpb.FATSz16;
		else
			fat->FATSz = dbr->fat32.FATSz32;
		
		if (0 != dbr->bpb.totSec16)
			fat->totSec = dbr->bpb.totSec16;
		else
			fat->totSec = dbr->bpb.totSec32;

		fat->firstRootDirSecNum = dbr->bpb.rsvdSecCnt + (dbr->bpb.numFATs * fat->FATSz);
		fat->rootDirSectors = ((dbr->bpb.rootEntCnt * 32) + (dbr->bpb.bytsPerSec - 1)) / dbr->bpb.bytsPerSec;
		
		fat->firstDataSector = dbr->bpb.rsvdSecCnt + (dbr->bpb.numFATs * fat->FATSz) + fat->rootDirSectors;
		fat->dataSec = fat->totSec - (dbr->bpb.rsvdSecCnt + (dbr->bpb.numFATs * fat->FATSz) - fat->rootDirSectors);

		{
			fat->countOfClusters = fat->dataSec / dbr->bpb.secPerClus;

			if (fat->countOfClusters < 4085)
				fat->type = _FAT12;
			else if (fat->countOfClusters < 65525)
				fat->type = _FAT16;
			else
				fat->type = _FAT32;

			if (_FAT16 == fat->type)
				fat->FATOffset = fat->countOfClusters * 2;
			else if (_FAT32 == fat->type)
				fat->FATOffset = fat->countOfClusters * 4;

			fat->FATSecNum = dbr->bpb.rsvdSecCnt + (fat->FATOffset / dbr->bpb.bytsPerSec);

			fat->FATEntOffset = (fat->FATOffset % dbr->bpb.bytsPerSec);

			char* secBuf = new char[512]();

			ReadDisk(secBuf, 1, fat->FATSecNum);

			if (_FAT16 == fat->type)
				fat->FAT16ClusEntryVal = *((uint32_t*)&secBuf[fat->FATEntOffset]);
			else if (_FAT32 == fat->type)
				fat->FAT32ClusEntryVal = (*((uint32_t*)&secBuf[fat->FATEntOffset])) & 0x0fffffff;
			
			delete[] secBuf;
		}

		return true;
	}

	return false;
}


/// @brief 
/// @param dirName 
/// @param flag 
/// @return 
void Fatfs::ShortNameLowedCase(char* name, int flag)
{
	//Name body (8 byte)
	if ((flag & _NS_BODY) == _NS_BODY)
	{
		for (uint8_t i = 0; i < 8; i++)
		{
			if (name[i] >= 'A' && name[i] <= 'Z') name[i] = name[i] + 0x20;
		}
	}

	//Name ext (3 byte)
	if ((flag & _NS_EXT) == _NS_EXT)
	{
		for (uint8_t i = 8; i < 11; i++)
		{
			if (name[i] >= 'A' && name[i] <= 'Z') name[i] = name[i] + 0x20;
		}
	}
}


/// @brief Get short name
/// @param dirName 
/// @param dir 
/// @return 
void Fatfs::GetShortName(char* dirName, FATShortDir* dir)
{
	uint8_t pos = 0;

	//8.3 name upper case -> lowed case
	ShortNameLowedCase(dir->name, dir->NTRes);

	//8.3 name body
	for (uint8_t i = 0; i < 8; i++)
	{
		if (' ' !=  dir->name[i])
		{
			dirName[pos++] = dir->name[i];
		} 
		else break;
	}

	//8.3 name dot
	if (' ' != dir->name[8]) dirName[pos++] = '.';

	//8.3 name ext
	for (uint8_t i = 8; i < 11; i++)
	{
		if (' ' != dir->name[i])
		{
			dirName[pos++] = dir->name[i]; 
		}
		else break;
	}

	//String EOC
	dirName[pos] = '\0';
}


/// @brief 
/// @param dirName 
/// @param dir 
/// @return 
void Fatfs::GetLongName(char* dirName, FATLongDir* dir)
{
	uint8_t pos = 0;
	uint8_t n = dir[0].ord - 0x40;
	
	//Loop for sequence of long directory entries
	while (n--)
	{
		//Part 1 of long name 
		for (uint8_t i = 0; i < 10; i += 2)
		{
			if (0xff != dir[n].name1[i])
			{
				dirName[pos++] = dir[n].name1[i];
			}
			else break;
		}

		//Part 2 of long name 
		for (uint8_t i = 0; i < 11; i += 2)
		{
			if (0xff != dir[n].name2[i])
			{
				dirName[pos++] = dir[n].name2[i];
			}
			else return;
		}

		//Part 3 of long name
		for (uint8_t i = 0; i < 4; i += 2)
		{
			if (0xff != dir[n].name3[i])
			{
				dirName[pos++] = dir[n].name3[i];
			}
			else return;
		}
	}
}


/// @brief 
/// @param data 
/// @param SecSize 
/// @param sector 
void Fatfs::ReadDisk(char* data, uint32_t SecSize, uint32_t sector)
{
	disk->Read((uint8_t*)data, SecSize, sector + mbr->dpt[0].relativeSectors);
}


/// @brief 
/// @param dirSecNum 
/// @param dirSecSize 
void Fatfs::ReadDir(uint32_t dirSecNum, uint32_t dirSecSize)
{
	char* secBuf = new char[512]();
	char* dirName = new char[100]();

	for (uint32_t sec = 0; sec < dirSecSize; sec++)
	{
		ReadDisk(secBuf, 1, dirSecNum + sec);
		uint8_t* buff = (uint8_t*)(secBuf);

		while (1)
		{
			FATLongDir*  longDir = (FATLongDir*)(buff);
			FATShortDir* shortDir;
			
			//Found an active long name sub-component.
			if (((longDir->attr & _ATTR_LONG_NAME_MASK) == _ATTR_LONG_NAME) && (longDir->ord != 0xE5))
			{
				uint8_t n = longDir->ord - 0x40;

				longDir  = new FATLongDir[n]();
				shortDir = new FATShortDir[1]();

				uint32_t needing = (32 * (n + 1));
				uint32_t remaining = 512 - ((uint32_t)buff - (uint32_t)secBuf);

				if (needing > remaining)
				{
					memcpy((void*)longDir, (const void*)buff, remaining);
					
					sec++;
					ReadDisk(secBuf, 1, dirSecNum + sec);
					buff = (uint8_t*)(secBuf);

					uint32_t read = needing - remaining - 32;
					if (read) memcpy((void*)(longDir + remaining), (const void*)buff, read);
					memcpy((void*)shortDir, (const void*)(buff + read), 32);
					
					buff += read;
				}
				else
				{
					memcpy((void*)longDir, (const void*)buff, 32 * n);
					memcpy((void*)shortDir, (const void*)(buff + n * 32), 32);
					
					buff += (32 * n);
				}

				if ((shortDir->attr & (_ATTR_DIRECTORY | _ATTR_VOLUME_ID)) == 0x00)
				{
					if (longDir->chksum == ChkSum(shortDir->name))
					{
						GetLongName(dirName, longDir);
						debug.Output(Debug::_Lv2, "Long name Found a file: %s", dirName);
					}
				}
				else if ((shortDir->attr & (_ATTR_DIRECTORY | _ATTR_VOLUME_ID)) == _ATTR_DIRECTORY)
				{
					if (longDir->chksum == ChkSum(shortDir->name))
					{
						GetLongName(dirName, longDir);
						debug.Output(Debug::_Lv2, "Long name Found a directory: %s", dirName);

						ReadDir(GetFirstSerctorOfCluster(MegreCluster(shortDir->fstClusHI, shortDir->fstClusLO)), 1);
					}
				}
				else if ((shortDir->attr & (_ATTR_DIRECTORY | _ATTR_VOLUME_ID)) == _ATTR_VOLUME_ID)
				{
					if (longDir->chksum == ChkSum(shortDir->name))
					{
						GetLongName(dirName, longDir);
						debug.Output(Debug::_Lv2, "Long name Found a volume label: %s", dirName);
					}
				}
				else
				{
					debug.Output(Debug::_Lv2, "Long name Found an invalid directory entry");
					break;
				}

				delete[] longDir;
				delete[] shortDir;
			}
			else if ((longDir->ord != 0) && (longDir->ord != 0xE5))
			{
				shortDir = (FATShortDir*)(buff);

				if ((shortDir->attr & (_ATTR_DIRECTORY | _ATTR_VOLUME_ID)) == 0x00)
				{
					GetShortName(dirName, shortDir);
					debug.Output(Debug::_Lv2, "Short name Found a file: %s", dirName);
				}
				else if ((shortDir->attr & (_ATTR_DIRECTORY | _ATTR_VOLUME_ID)) == _ATTR_DIRECTORY)
				{
					GetShortName(dirName, shortDir);
					debug.Output(Debug::_Lv2, "Short name Found a directory: %s", dirName);

					if (0 != strcmp(dirName, ".") && 0 != strcmp(dirName, ".."))
					{
						ReadDir(GetFirstSerctorOfCluster(MegreCluster(shortDir->fstClusHI, shortDir->fstClusLO)), 1);
					}
				}
				else if ((shortDir->attr & (_ATTR_DIRECTORY | _ATTR_VOLUME_ID)) == _ATTR_VOLUME_ID)
				{
					GetShortName(dirName, shortDir);
					debug.Output(Debug::_Lv2, "Short name Found a volume label: %s", dirName);
				}
				else
				{
					debug.Output(Debug::_Lv2, "Short name Found an invalid directory entry");
					break;
				}
			}

			buff += 32;
			if (((uint32_t)buff - (uint32_t)secBuf) >= 512) break;
		}
	}

	delete[] secBuf;
	delete[] dirName;
}


/// @brief 
/// @param clusHI 
/// @param clusLO 
/// @return 
uint32_t Fatfs::MegreCluster(uint16_t clusHI, uint16_t clusLO)
{
	return (uint32_t)clusHI << 16 | clusLO;
}


/// @brief 
/// @param n 
/// @return 
uint32_t Fatfs::GetFirstSerctorOfCluster(uint16_t cluster)
{
	return ((cluster - 2) * dbr->bpb.secPerClus) + fat->firstDataSector;
}


/// @brief List dir
void Fatfs::ListDir()
{
	ReadDir(fat->firstRootDirSecNum, fat->rootDirSectors);
}


/// @brief Fatfs mount
/// @return 
int Fatfs::Mount(const char* path, const char* mount, int opt)
{
	disk = device.GetDriver(DriverID::_storage + 1);
	
	if (NULL == disk)
	{
		debug.Error("Not disk driver found");
		return _ERR;
	}

	if (false == ReadMBR())
	{
		debug.Error("%s not a valid disk", path);
		return _ERR;
	}

	if (false == ReadDBR())
	{
		debug.Error("Not DBR found");
		return _ERR;
	}

	if (false == CheckFS())
	{
		debug.Error("Not filesystem found");
		return _ERR;
	}

	ListDir();

	debug.Output(Debug::_Lv2, "%s -> %s mount successful", path, mount);
	return _OK;
}


/// @brief Fatfs unmount
/// @return 
int Fatfs::Unmount(const char* mount)
{
	return _OK;
}


/// @brief Fatfs open
/// @param name 
/// @param mode 
/// @return 
int Fatfs::Open(const char* name, int mode)
{
	return 0;
}


/// @brief Fatfs write
/// @param data 
/// @param size 
/// @param offset 
/// @return 
int Fatfs::Write(char* data, int size, int offset)
{
	return 0;
}


/// @brief Fatfs read
/// @param data 
/// @param size 
/// @param offset 
/// @return 
int Fatfs::Read(char* data, int size, int offset)
{
	return 0;
}


/// @brief Fatfs seek
/// @param offset 
/// @return 
int Fatfs::Seek(int offset)
{
	return 0;
}


/// @brief Fatfs rename
/// @param old 
/// @param now 
/// @return 
int Fatfs::Rename(const char* old, const char* now)
{
	return 0;
}


/// @brief Fatfs copy
/// @param from 
/// @param to 
/// @return 
int Fatfs::Copy(const char* from, const char* to)
{
	return 0;
}


/// @brief Fatfs remove
/// @return 
int Fatfs::Remove()
{
	return 0;
}


/// @brief Fatfs size
/// @return 
int Fatfs::Size()
{
	return 0;
}


/// @brief Fatfs close
/// @return 
int Fatfs::Close()
{
	return 0;
}


///Register file system
REGISTER_FS(new Fatfs(), fatfs);
