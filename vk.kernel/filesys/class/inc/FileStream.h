//###########################################################################
// FileStream.h
// Declarations of the functions that manage file
//
// $Copyright: Copyright (C) village
//###########################################################################
#ifndef __FILE_STREAM_H__
#define __FILE_STREAM_H__

#include "Module.h"
#include "FileOpts.h"


/// @brief FileStream
class FileStream
{
private:
	//Members
	int fd;
	FileOpts* opts;
public:
	//Methods
	FileStream(const char* name = NULL, int mode = 0);
	~FileStream();
	bool IsExist(const char* name);
	int Open(const char* name, int mode);
	int Write(char* data, int size, int offset = 0);
	int Read(char* data, int size, int offset = 0);
	int Size();
	void Close();
};

#endif //!__FILE_STREAM_H__
