###########################################################################
# Makefile
# The Top Makefile of village-target project
#
# $Copyright: Copyright (C) village
############################################################################
-include .config

######################################
# target
######################################
TARGET := village

######################################
# building variables
######################################
# optimization
OPT ?= $(CONFIG_OPT:"%"=%)

# debug build?
ifeq ($(CONFIG_DEBUG), y)
  DEBUG ?= 1
else
  DEBUG ?= 0
endif

# silence build
ifeq ($(CONFIG_VERBOSE_MODE), y)
  Q = 
else
  Q = @
endif

export Q


#######################################
# paths
#######################################
# Build path
BUILD_DIR   := vk.build
MODULES_DIR := $(BUILD_DIR)/modules


######################################
# include other makefile
######################################
-include vk.application/Makefile
-include vk.bootloader/Makefile
-include vk.hardware/Makefile
-include vk.kernel/Makefile
-include vk.gui/Makefile


#######################################
# tasks
#######################################
# default action: build all
all:
	$(Q)$(MAKE) bootsection
	$(Q)$(MAKE) bootloader
	$(Q)$(MAKE) kernel
	$(Q)$(MAKE) module
	$(Q)$(MAKE) osImage

# flash firmware
flash:
	openocd $(FLASH_CFG) -c "program $(BUILD_DIR)/$(TARGET).bin verify reset exit 0x08000000"


#######################################
# cross compile tool
#######################################
GCC_PREFIX ?= $(CONFIG_CROSS_COMPILE:"%"=%)
CXX = $(GCC_PREFIX)g++
CC  = $(GCC_PREFIX)gcc
AS  = $(GCC_PREFIX)gcc -x assembler-with-cpp
AR  = $(GCC_PREFIX)ar
LD  = $(GCC_PREFIX)ld
CP  = $(GCC_PREFIX)objcopy
SZ  = $(GCC_PREFIX)size
ST  = $(GCC_PREFIX)strip
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S


#######################################
# setting build environment
#######################################
INCLUDES  = $(addprefix -I, $(inc-y) $(inc-m))
VPATH     = $(addprefix $(BUILD_DIR)/, $(src-y) $(src-m)) $(src-y) $(src-m)


#######################################
# compiler flags
#######################################
ifeq ($(DEBUG), 1)
CFLAGS    += -g -gdwarf-2 -DDebug
endif

ifneq ($(CONFIG_ENVIRONMENT), y)
CFLAGS    += -DKBUILD_NO_ENVIRONNEMNT
endif


#######################################
# mixed compiling
#######################################
%.o: %.s
	$(Q)echo Compiling $@
	$(Q)mkdir -p $(BUILD_DIR)/$(dir $<)
	$(Q)$(AS) -c $(CFLAGS) $< -o $(BUILD_DIR)/$(dir $<)/$@

%.o: %.c
	$(Q)echo Compiling $@
	$(Q)mkdir -p $(BUILD_DIR)/$(dir $<)
	$(Q)$(CC) -c $(CFLAGS) $< -o $(BUILD_DIR)/$(dir $<)/$@

%.o: %.cpp
	$(Q)echo Compiling $@
	$(Q)mkdir -p $(BUILD_DIR)/$(dir $<)
	$(Q)$(CXX) -c $(CXXFLAGS) $< -o $(BUILD_DIR)/$(dir $<)/$@

%.hex: %.elf
	$(Q)echo output $@
	$(Q)$(HEX) $< $@

%.bin: %.elf
	$(Q)echo output $@
	$(Q)$(BIN) $< $@


#######################################
# build the bootsection
#######################################
ifeq ($(CONFIG_BOOTSECTION), y)
bootsection: $(objs-bs-y)
	$(Q)$(MAKE) \
	$(BUILD_DIR)/$(TARGET)-bs.elf \
	$(BUILD_DIR)/$(TARGET)-bs.hex \
	$(BUILD_DIR)/$(TARGET)-bs.bin

$(BUILD_DIR)/$(TARGET)-bs.elf: $(objs-bs-y)
	$(Q)echo output $@
	$(Q)$(LD) $(BSLDFLAGS) $^ -o $@
	$(Q)$(SZ) $@
else
bootsection:
endif


#######################################
# build the bootloader
#######################################
ifeq ($(CONFIG_BOOTLOADER), y)
bootloader: $(objs-bl-y)
	$(Q)$(MAKE) \
	$(BUILD_DIR)/$(TARGET)-bl.elf \
	$(BUILD_DIR)/$(TARGET)-bl.hex \
	$(BUILD_DIR)/$(TARGET)-bl.bin

$(BUILD_DIR)/$(TARGET)-bl.elf: $(objs-bl-y)
	$(Q)echo output $@
	$(Q)$(LD) $(BLDFLAGS) $^ -o $@
	$(Q)$(SZ) $@
else
bootloader:
endif


#######################################
# build the modules
#######################################
ifeq ($(CONFIG_MODULE), y)
module: $(objs-m)
	$(Q)mkdir -p $(MODULES_DIR)
	$(Q)$(MAKE) $(addprefix $(MODULES_DIR)/, $(objs-m:.o=.mo))

$(MODULES_DIR)/%.mo: %.o
	$(Q)echo Generating $(notdir $@)
	$(Q)$(LD) $(MLDFLAGS) $< -o $(@:.mo=.elf)
	$(Q)$(ST) -g -o $@ $(@:.mo=.elf)
else
module:
endif


#######################################
# build the kernel
#######################################
ifeq ($(CONFIG_KERNEL), y)
kernel: $(objs-y)
	$(Q)$(MAKE) \
	$(BUILD_DIR)/$(TARGET)-kernel.elf \
	$(BUILD_DIR)/$(TARGET)-kernel.hex \
	$(BUILD_DIR)/$(TARGET)-kernel.bin

$(BUILD_DIR)/$(TARGET)-kernel.elf: $(objs-y)
	$(Q)echo output $@
	$(Q)$(LD) $(KLDFLAGS) $^ -o $@
	$(Q)$(SZ) $@
else
kernel:
endif


#######################################
# generate the os image
#######################################
osImage:
	dd if=/dev/zero                   of=$(BUILD_DIR)/village-os.img bs=512 count=18
	dd if=$(BUILD_DIR)/village-bs.bin of=$(BUILD_DIR)/village-os.img bs=512 seek=0 conv=notrunc
	dd if=$(BUILD_DIR)/village-bl.bin of=$(BUILD_DIR)/village-os.img bs=512 seek=1 conv=notrunc
#	cat $(BUILD_DIR)/village-os.img $(BUILD_DIR)/village-kernel.bin > $(BUILD_DIR)/village-os.img


#######################################
# menuconfig
#######################################
Scripts      := ./vk.scripts
Kconfig      := ./Kconfig

menuconfig: $(Scripts)/kconfig/mconf
	$(Q)$< $(Kconfig)

silentoldconfig: $(Scripts)/kconfig/conf
	$(Q)mkdir -p include/config include/generated
	$(Q)$< -s --$@ $(Kconfig)

$(Scripts)/kconfig/mconf:
	$(Q)$(MAKE) -C $(Scripts)/kconfig

$(Scripts)/kconfig/conf:
	$(Q)$(MAKE) -C $(Scripts)/kconfig


#######################################
# clean up
#######################################
clean:
	$(Q)rm -rf $(BUILD_DIR)
	$(Q)rm -rf $(MODULES_DIR)
	$(Q)rm -rf include

modclean:
	$(Q)rm -rf $(MODULES_DIR)

distclean: clean
	$(Q)$(MAKE) -C $(Scripts)/kconfig clean
