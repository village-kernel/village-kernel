###########################################################################
# Makefile
# The Makefile of vk.kernel
#
# $Copyright: Copyright (C) village
############################################################################

######################################
# includes
######################################
INCLUDES-y += \
	-I./vk.kernel/kernel/if \
	-I./vk.kernel/kernel/inc \

INCLUDES-$(CONFIG_FATFS) += -I./vk.kernel/libraries/fatfs/inc
INCLUDES-$(CONFIG_CONSOLE) += -I./vk.kernel/console/inc
INCLUDES-$(CONFIG_UTILITIES) += -I./vk.kernel/utilities/inc


######################################
# sources
######################################
# ASM sources
ASM_SOURCES-y += 

# C sources
C_SOURCES-y +=

# Cpp sources
CPP_SOURCES-y += $(wildcard vk.kernel/kernel/src/*.cpp)

CPP_SOURCES-$(CONFIG_FATFS) += $(wildcard vk.kernel/libraries/fatfs/src/*.cpp)

CPP_SOURCES-$(CONFIG_FILESTREAM) += vk.kernel/utilities/src/FileStream.cpp
CPP_SOURCES-$(CONFIG_INIPARSER) += vk.kernel/utilities/src/iniParser.cpp
CPP_SOURCES-$(CONFIG_PINPARSER) += vk.kernel/utilities/src/pinParser.cpp

CPP_SOURCES-$(CONFIG_CMD_ABOUT) += vk.kernel/console/src/CmdAbout.cpp
CPP_SOURCES-$(CONFIG_CMD_HELP) += vk.kernel/console/src/CmdHelp.cpp
CPP_SOURCES-$(CONFIG_CONSOLE) += vk.kernel/console/src/CmdMsgMgr.cpp
CPP_SOURCES-$(CONFIG_CONSOLE) += vk.kernel/console/src/Console.cpp
