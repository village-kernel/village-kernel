//###########################################################################
// FileStream.cpp
// Definitions of the functions that manage file
//
// $Copyright: Copyright (C) village
//###########################################################################
#include "FileStream.h"
#include "Kernel.h"


/// @brief Constructor
FileStream::FileStream(const char* name, int mode)
	:fd(-1),
	opts(NULL)
{
	filesys = (FileSystem*)kernel->feature.GetModule("fileSystem");
	if (NULL == filesys)
	{
		kernel->debug.Error("file system feature not support");
		return;
	}
	if (NULL != name) Open(name, mode);
}


/// @brief Destructor
FileStream::~FileStream()
{
}


/// @brief FileStream is exist
/// @param name 
/// @return 
bool FileStream::IsExist(const char* name)
{
	if (NULL != filesys)
	{
		opts = filesys->GetFileOpts(name);
	}

	if (NULL != opts)
	{
		fd = opts->Open(name, FileMode::_OpenExisting);
	}
	
	if (fd != -1)
	{
		opts->Close(fd);
		return true;
	}

	return false;
}


/// @brief FileStream open
/// @param name 
/// @param opt 
/// @return 
bool FileStream::Open(const char* name, int mode)
{
	if (NULL != filesys)
	{
		opts = filesys->GetFileOpts(name);
	}

	if (NULL != opts)
	{
		fd = opts->Open(name, mode);
	}

	return (fd != -1);
}


/// @brief FileStream write
/// @param data 
/// @param size 
/// @param offset 
/// @return 
int FileStream::Write(char* data, int size, int offset)
{
	if (NULL != opts)
	{
		return opts->Write(fd, data, size, offset);
	}
	return 0;
}


/// @brief FileStream read
/// @param data 
/// @param size 
/// @param offset 
/// @return 
int FileStream::Read(char* data, int size, int offset)
{
	if (NULL != opts)
	{
		return opts->Read(fd, data, size, offset);
	}
	return 0;
}


/// @brief FileStream size
/// @return 
int FileStream::Size()
{
	if (NULL != opts)
	{
		return opts->Size(fd);
	}
	return 0;
}


/// @brief FileStream close
void FileStream::Close()
{
	if (NULL != opts)
	{
		opts->Close(fd);
	}
}
