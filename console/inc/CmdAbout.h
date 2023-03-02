//###########################################################################
// CmdAbout.h
// Declarations of the functions that manage command about
//
// $Copyright: Copyright (C) village
//###########################################################################
#ifndef __CMD_ABOUT_H__
#define __CMD_ABOUT_H__

#include "CmdMsgMgr.h"

///CmdAbout
class CmdAbout
{
private:
	//Members
	CmdMsgMgr* msgMgr;
public:
	//Methods
	void Initialize(CmdMsgMgr* msgMgr);
	void SetArgs(uint8_t* args);
	void Execute();
};

#endif // !__CMD_ABOUT_H__
