//###########################################################################
// FatObject.h
// Declarations of the functions that manage fat file system
//
// $Copyright: Copyright (C) village
//###########################################################################
#ifndef __FAT_OBJECT_H__
#define __FAT_OBJECT_H__

#include "stdint.h"
#include "string.h"


/// @brief FatObject
class FatObject
{
public:
	//Enumerates
	enum DirAttr
	{
		_ATTR_FILE       = 0x00,
		_ATTR_READ_ONLY  = 0x01,
		_ATTR_HIDDEN     = 0x02,
		_ATTR_SYSTEM     = 0x04,
		_ATTR_VOLUME_ID  = 0x08,
		_ATTR_DIRECTORY  = 0x10,
		_ATTR_ARCHIVE    = 0x20,
		_ATTR_LONG_NAME  = _ATTR_READ_ONLY | _ATTR_HIDDEN | _ATTR_SYSTEM | _ATTR_VOLUME_ID,
		_ATTR_LONG_NAME_MASK = _ATTR_READ_ONLY | _ATTR_HIDDEN | _ATTR_SYSTEM | _ATTR_VOLUME_ID | _ATTR_DIRECTORY | _ATTR_ARCHIVE,
	};

	enum NSFlag
	{
		_NS_NONE     = 0x00,
		_NS_LOSS     = 0x01,   /* Out of 8.3 format */
		_NS_LFN      = 0x02,   /* Force to create LFN entry */
		_NS_LAST     = 0x04,   /* Last segment */
		_NS_BODY     = 0x08,   /* Lower case flag (body) */
		_NS_EXT      = 0x10,   /* Lower case flag (ext) */
		_NS_DOT      = 0x20,   /* Dot entry */
		_NS_NOLFN    = 0x40,   /* Do not find LFN */
		_NS_NONAME   = 0x80,   /* Not followed */
	};

	//Structures
	struct LongEntry
	{
		//Members
		uint8_t  ord;
		uint16_t name1[5];
		uint8_t  attr;
		uint8_t  type;
		uint8_t  chksum;
		uint16_t name2[6];
		uint16_t fstClustLO;
		uint16_t name3[2];

		//Methods
		void Fill();
	} __attribute__((packed));

	struct ShortEntry
	{
		//Members
		char     name[11];
		uint8_t  attr;
		uint8_t  NTRes;
		uint8_t  crtTimeTenth;
		uint16_t crtTime;
		uint16_t crtDate;
		uint16_t lstAccDate;
		uint16_t fstClustHI;
		uint16_t wrtTime;
		uint16_t wrtDate;
		uint16_t fstClustLO;
		uint32_t fileSize;

		//Methods
		ShortEntry();
	} __attribute__((packed));

	union UnionEntry
	{
		//Members
		LongEntry  lfe;
		ShortEntry sfe;
		
		//Methods
		UnionEntry();
		bool IsDirectory();
		bool IsVolume();
		bool IsFile();
		bool IsHidden();
		bool IsLongName();
		bool IsValid();
		uint8_t OrdSize();
		uint8_t AllocSize();
	} __attribute__((packed));
private:
	//Static constants
	static const uint8_t long_name_size  = 13;
	static const uint8_t short_name_size = 11;
	static const uint8_t volume_label_size = 11;
	static const uint8_t dir_seq_flag = 0x40;
	static const uint8_t dir_free_flag = 0xe5;

	//Methods
	uint8_t ChkSum(const char* name);
public:
	//Record members
	uint32_t clust;
	uint32_t sector;
	uint32_t index;
	uint32_t size;

	//Data members
	LongEntry*  lfe;
	ShortEntry* sfe;
	UnionEntry* ufe;
public:
	//Methods
	FatObject(char* raw = new char[32]());
	~FatObject();

	void Setup(char* raw);
	void SetEntryFree();

	void GenNumName(int num);
	void SetShortName(const char* name);
	char* GetShortName();
	void SetLongName(const char* name);
	char* GetLongName();
	void SetVolumeLabel(const char* label);
	char* GetVolumeLabel();
	void SetAttribute(uint8_t attr);
	uint8_t GetAttribute();
	void SetNTRes(uint8_t NTRes);
	uint8_t GetNTRes();
	void SetCreateTenth(uint16_t tenth);
	uint16_t GetCreateTenth();
	void SetCreateTime(uint16_t time);
	uint16_t GetCreateTime();
	void SetCreateDate(uint16_t date);
	uint16_t GetCreateDate();
	void SetLastAccDate(uint16_t date);
	uint16_t GetLastAccDate();
	void SetWriteTime(uint16_t time);
	uint16_t GetWriteTime();
	void SetWriteDate(uint16_t date);
	uint16_t GetWriteDate();
	void SetFirstCluster(uint32_t clust);
	uint32_t GetFirstCluster();
	void SetFileSize(uint32_t size);
	uint32_t GetFileSize();
};

#endif //!__FAT_OBJECT_H__
