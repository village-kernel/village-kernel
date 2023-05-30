###########################################################################
# Makefile
# The Makefile of vk.aplication
#
# $Copyright: Copyright (C) village
############################################################################

######################################
# paths
######################################
inc-y += vk.application/demo/inc
src-y += vk.application/demo/src

######################################
# objects
######################################
objs-$(CONFIG_APPLICATION1) += Application1.o
objs-$(CONFIG_APPLICATION2) += Application2.o
objs-$(CONFIG_APPLICATION3) += Application3.o
objs-$(CONFIG_APPLICATION4) += Application4.o
objs-$(CONFIG_APPLICATION5) += Application5.o
objs-$(CONFIG_APPLICATION6) += Application6.o
