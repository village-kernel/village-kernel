//###########################################################################
// FileDefs.h
// Declarations of the interface file system
//
// $Copyright: Copyright (C) village
//###########################################################################
#ifndef __FILE_DEFINES_H__
#define __FILE_DEFINES_H__


/// @brief FileMode
enum FileMode
{
	_OpenExisting  = 0x00,
	_Read          = 0x01,
	_Write         = 0x02,
	_CreateNew     = 0x04,
	_CreateAlways  = 0x10,
	_OpenAppend    = 0x30,
};


/// @brief FileType
enum FileType
{
	_Unknown  = 0x00,
	_File     = 0x01,
	_Diretory = 0x02,
	_Volume   = 0x04,
};


/// @brief FileAttr
enum FileAttr
{
	_Visible = 0x04,
	_Hidden  = 0x08,
	_System  = 0x10,
};


/// @brief FileDir
struct FileDir
{
	char* path;
	char* name;
	FileType type;
	FileAttr attr;

	FileDir()
		:path(NULL),
		name(NULL),
		type(_Unknown),
		attr(_Visible)
	{
	}

	~FileDir()
	{
		delete[] path;
		delete[] name;
	}
};


/// @brief MountNode
struct MountNode
{
	char*    target;
	char*    source;
	uint16_t access;

	MountNode(char* target, char* source, uint16_t access)
		:target(target),
		source(source),
		access(access)
	{}
};


///Command register macro
#define REGISTER_FS(fs, name)                                 \
static struct _FS_##name {                                    \
	_FS_##name() {                                            \
		FileSystem* filesys = (FileSystem*)Kernel::modular.GetModule("fileSystem"); \
		if (NULL == filesys)                                  \
		{                                                     \
			Kernel::debug.Error("file system feature not support");\
			return;                                           \
		}                                                     \
		filesys->RegisterFS(fs, #name);                       \
	}                                                         \
	~_FS_##name() {                                           \
		FileSystem* filesys = (FileSystem*)Kernel::modular.GetModule("fileSystem"); \
		if (NULL == filesys)                                  \
		{                                                     \
			Kernel::debug.Error("file system feature not support");\
			return;                                           \
		}                                                     \
		filesys->DeregisterFS(fs, #name);                     \
	}                                                         \
} const _fs_##name __attribute__((used,__section__(".fs")))


#endif //!__FILE_DEFINES_H__
