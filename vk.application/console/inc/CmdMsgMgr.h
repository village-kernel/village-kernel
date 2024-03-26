//###########################################################################
// CmdMsgMgr.h
//
// Decodes incoming bytes into messages and Encodes outgoing messages
//
// $Copyright: Copyright (C) village
//###########################################################################
#ifndef __COMMAND_MSG_MGR_H__
#define __COMMAND_MSG_MGR_H__

#include "DrvStream.h"


//CmdMsg struct
struct CmdMsg
{
	uint8_t* cmd;
	uint8_t* args;
};


///CmdMsg
class CmdMsgMgr
{
private:
	//Enumerations
	enum InputMode
	{
		_EDIT = 0,
		_INSERT,
		_ANSI,
		_DCS,
		_CSI,
		_OSC,
	};

	//Static constants
	static const uint16_t cmd_history_size = 10;
	static const uint16_t cmd_buffer_size = 256;
	static const uint16_t arg_buffer_size = 256;

	//Members
	CmdMsg rxMsg;
	uint16_t txBufPos;
	uint16_t rxBufPos;
	uint8_t history;
	uint8_t cmdBuffer[cmd_buffer_size] = { 0 };
	uint8_t txBuffer[arg_buffer_size] = { 0 };
	uint8_t rxBuffer[arg_buffer_size] = { 0 };
	uint8_t* cmdHistory[cmd_history_size] = { NULL };
	DrvStream transceiver;
	InputMode inputMode;

	//Methods
	void RecordTempCmd();
	void RecordHistory();
	void RestoredHistory();
	bool HandleInputData();
public:
	//Methods
	CmdMsgMgr();
	void Initialize(const char* driver);
	bool Execute();
	void Write(uint8_t* txMsg, uint16_t size = 0);

	///Gets the decoded message
	inline CmdMsg Read() { return rxMsg; }
};

#endif //!__COMMAND_MSG_MGR_H__
