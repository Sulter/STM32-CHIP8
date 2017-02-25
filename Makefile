##
## This file is part of the libopencm3 project.
##
## Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
##
## This library is free software: you can redistribute it and/or modify
## it under the terms of the GNU Lesser General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public License
## along with this library.  If not, see <http://www.gnu.org/licenses/>.
##

OUR_SRC_DIR = src
OUR_INCLUDE_DIR = include
OUR_BUILD_DIR = src

BINARY = $(OUR_SRC_DIR)/main
OBJS = $(OUR_SRC_DIR)/pcd8544.o $(OUR_SRC_DIR)/timer.o $(OUR_SRC_DIR)/usb_cdc.o $(OUR_SRC_DIR)/chip8.o $(OUR_SRC_DIR)/rng.o

# Default arch
ifndef ARCH
$(warning arch defaults to stm32)
ARCH=stm32
V=1
endif

############### MAKING FOR X86
ifeq ($(ARCH), x86)
$(info making for X86 with SDL)

include x86SDL.mk

endif


############ MAKING FOR STM32
ifeq ($(ARCH), stm32)
$(info making for STM32)


OPENCM3_DIR = libopencm3
LDSCRIPT = stm32f4-discovery.ld

include libopencm3.target.mk

endif
