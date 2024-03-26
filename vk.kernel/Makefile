###########################################################################
# Makefile
# The Makefile of vk.kernel
#
# $Copyright: Copyright (C) village
############################################################################

######################################
# libraries
######################################
libs-$(CONFIG_KERNEL)                       += kernel


######################################
# paths
######################################
inc-y                                       += vk.kernel/core/if
inc-y                                       += vk.kernel/core/inc

src-y                                       += vk.kernel/core/src

inc-$(CONFIG_X86)                           += vk.kernel/arch/x86/bios/inc
inc-$(CONFIG_CORTEX_M)                      += vk.kernel/arch/arm/cortex-m/inc
inc-$(CONFIG_STM32F4xx)                     += vk.kernel/arch/arm/cortex-m/inc/stm32f4xx
inc-$(CONFIG_STM32H7xx)                     += vk.kernel/arch/arm/cortex-m/inc/stm32h7xx

src-$(CONFIG_X86)                           += vk.kernel/arch/x86/bios/src
src-$(CONFIG_CORTEX_M)                      += vk.kernel/arch/arm/cortex-m/src
src-$(CONFIG_STM32F4xx)                     += vk.kernel/arch/arm/cortex-m/src/stm32f4xx
src-$(CONFIG_STM32H7xx)                     += vk.kernel/arch/arm/cortex-m/src/stm32h7xx

inc-$(CONFIG_SUBSYS)                        += vk.kernel/subsys/inc
src-$(CONFIG_SUBSYS)                        += vk.kernel/subsys/src

inc-$(CONFIG_FILESYSTEM)                    += vk.kernel/filesys/core/inc
inc-$(CONFIG_FATFS)                         += vk.kernel/filesys/fat/inc

src-$(CONFIG_FILESYSTEM)                    += vk.kernel/filesys/core/src
src-$(CONFIG_FATFS)                         += vk.kernel/filesys/fat/src

inc-$(CONFIG_FILEUTILS)                     += vk.kernel/utilities/fileutils/inc
inc-$(CONFIG_BINUTILS)                      += vk.kernel/utilities/binutils/inc
inc-$(CONFIG_PARSER)                        += vk.kernel/utilities/parser/inc
inc-$(CONFIG_SYNC)                          += vk.kernel/utilities/sync/inc

src-$(CONFIG_FILEUTILS)                     += vk.kernel/utilities/fileutils/src
src-$(CONFIG_BINUTILS)                      += vk.kernel/utilities/binutils/src
src-$(CONFIG_PARSER)                        += vk.kernel/utilities/parser/src
src-$(CONFIG_SYNC)                          += vk.kernel/utilities/sync/src

######################################
# objects
######################################
objs-y                                      += Village.o
objs-y                                      += Debug.o
objs-y                                      += Symbol.o
objs-y                                      += Feature.o
objs-y                                      += Device.o
objs-y                                      += Memory.o
objs-y                                      += Thread.o
objs-y                                      += Interrupt.o

objs-y                                      += ArchInterrupt.o
objs-y                                      += Exception.o
objs-y                                      += Scheduler.o
objs-y                                      += System.o

objs-$(CONFIG_INPUT)                        += Input.o
objs-$(CONFIG_LOADER)                       += Loader.o
objs-$(CONFIG_WORKQUEUE)                    += WorkQueue.o

objs-$(CONFIG_FILESYSTEM)                   += FileSystem.o
objs-$(CONFIG_FATFS)                        += FatDiskio.o
objs-$(CONFIG_FATFS)                        += FatEntry.o
objs-$(CONFIG_FATFS)                        += FatObject.o
objs-$(CONFIG_FATFS)                        += FatSystem.o
objs-$(CONFIG_FATFS)                        += FatVolume.o

objs-kernel-$(CONFIG_DIRSTREAM)             += DirStream.o
objs-kernel-$(CONFIG_DRVSTREAM)             += DrvStream.o
objs-kernel-$(CONFIG_FILESTREAM)            += FileStream.o
objs-kernel-$(CONFIG_FILESYSOPT)            += FileSysOpt.o

objs-kernel-$(CONFIG_SPINLOCK)              += SpinLock.o
objs-kernel-$(CONFIG_MUTEX)                 += Mutex.o
objs-kernel-$(CONFIG_SEMAPHORE)             += Semaphore.o

objs-kernel-$(CONFIG_BINUTILS)              += BaseExecutor.o
objs-kernel-$(CONFIG_BINLOADER)             += BinLoader.o
objs-kernel-$(CONFIG_BINEXECUTOR)           += BinExecutor.o
objs-kernel-$(CONFIG_ELFLOADER)             += ElfLoader.o
objs-kernel-$(CONFIG_ELFEXECUTOR)           += ElfExecutor.o
objs-kernel-$(CONFIG_LIBTOOL)               += LibraryTool.o
objs-kernel-$(CONFIG_MODTOOL)               += ModuleTool.o

objs-kernel-$(CONFIG_INIPARSER)             += iniParser.o
objs-kernel-$(CONFIG_PINPARSER)             += pinParser.o
objs-kernel-$(CONFIG_RCPARSER)              += rcParser.o
objs-kernel-$(CONFIG_REGEX)                 += Regex.o


#######################################
# compiler flags
#######################################
ifdef CONFIG_TASK_STACK
CFLAGS    += -DTASK_STACK=$(CONFIG_TASK_STACK)
endif
