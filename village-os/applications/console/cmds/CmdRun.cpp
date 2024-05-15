//###########################################################################
// CmdRun.cpp
// Definitions of the functions that manage command Run
//
// $Copyright: Copyright (C) village
//###########################################################################
#include "Cmd.h"
#include "Console.h"
#include "ElfExecutor.h"
#include "string.h"


///CmdRun
class CmdRun : public Cmd
{
private:
	//Members
	ElfExecutor elfExecutor;
	ElfExecutor::Behavior behavior;
public:
	/// @brief Constructor
	CmdRun() :behavior(ElfExecutor::_Foreground) {}

	/// @brief Cmd Run execute
	void Execute(int argc, char* argv[])
	{
		if (argc < 2)
		{
			console.Output("Usage: run [-b] <program> [arg1] [arg2] [...]");
			return;
		}

		behavior = ElfExecutor::_Foreground;

		for (int i = 0; i < argc; i++)
		{
			if (0 == strcmp(argv[i], "-b"))
			{
				behavior = ElfExecutor::_Background;
				argc -= (i + 1);
				argv += (i + 1);
				break;
			}
		}

		elfExecutor.Run(behavior, argv[1], argc - 1, argv + 1);
	}
};


///Register cmd
REGISTER_CMD(new CmdRun(), run);
