#
# Makefile for making the decode demos on DM6467 platform
#
# Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/
#
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions
#  are met:
#
#    Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#
#    Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the
#    distribution.
#
#    Neither the name of Texas Instruments Incorporated nor the names of
#    its contributors may be used to endorse or promote products derived
#    from this software without specific prior written permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
#  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
#  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
#  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
#  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
#  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
#  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
#  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
#  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
#  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
#  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

TARGET = $(notdir $(CURDIR))

ifeq ($(VERBOSE), true)
    override VERBOSE =
else
    override VERBOSE = @
endif

-include $(DMAI_INSTALL_DIR)/Platform.make

# Package path for the XDC tools
XDC_PATH = $(USER_XDC_PATH);../../packages;$(DMAI_INSTALL_DIR)/packages;$(CE_INSTALL_DIR)/packages;$(FC_INSTALL_DIR)/packages;$(LINK_INSTALL_DIR)/packages;$(LINK_INSTALL_DIR);$(XDAIS_INSTALL_DIR)/packages;$(CMEM_INSTALL_DIR)/packages;$(CODEC_INSTALL_DIR)/packages;$(CE_INSTALL_DIR)/examples

# Where to output configuration files
XDC_CFG		= $(TARGET)_config

# Output compiler options
XDC_CFLAGS	= $(XDC_CFG)/compiler.opt

# Output linker file
XDC_LFILE	= $(XDC_CFG)/linker.cmd

# Input configuration file
XDC_CFGFILE	= $(TARGET).cfg

PLATFORM_OS		     = $(strip $(PLATFORM_OS_dm6467_al))
PLATFORM_DEVICE		 = $(strip $(PLATFORM_DEVICE_dm6467_al))
PLATFORM_PERIPHERALS = $(strip $(PLATFORM_PERIPHERALS_dm6467_al))
PLATFORM_XDC_TARGET	 = $(strip $(PLATFORM_XDC_TARGET_dm6467_al))
PLATFORM_XDC		 = $(strip $(PLATFORM_XDC_dm6467_al))

#$(shell /bin/pwd)
NEW_HY_PATH = ./../hy_proj

NEW_HY_LIB_PATH = -L$(NEW_HY_PATH)/lib -lhy_io   \
				  -lhy_config\
				  -lhy_base\
				  -lhy_ptz\
                  -L$(NEW_HY_PATH)/ptz/lib \
		  -lov_ptz\
		  -L$(NEW_HY_PATH)/ptz/lib/uuid \
		  -luuid\
		  -L$(NEW_HY_PATH)/ptz/lib/openssl \
                  -lssl\
                  -lcrypto\
                  -L$(NEW_HY_PATH)/ptz/lib/ffmpeg \
                  -lpthread\
                  -lavformat \
                  -lavcodec \
                  -lavutil\
                  -lswresample\
                  -lswscale\
          -L$(NEW_HY_PATH)/3rd/lib -lsqlite3 -lzlog -ljpeg



HY_NEED_INCLUDE = -I$(NEW_HY_PATH)/base/inc \
				  -I$(NEW_HY_PATH)/io/inc \
				  -I$(NEW_HY_PATH)/config/inc    \
				  -I$(NEW_HY_PATH)/ptz/inc    \
				  -I$(NEW_HY_PATH)/ptz/inc/ffmpeg    \
				  -I$(NEW_HY_PATH)/ptz/inc/onvif    \
				  -I$(NEW_HY_PATH)/3rd/inc


# The XDC configuration tool command line
CONFIGURO = $(XDC_INSTALL_DIR)/xs xdc.tools.configuro

C_FLAGS += -Wall -g -DWITH_OPENSSL -DWITH_DOM -D_REENTRANT -D_GNU_SOURCE=1
C_FLAGS += $(HY_NEED_INCLUDE)

#LD_FLAGS += $(NEW_HY_LIB_PATH)
LD_FLAGS += -lpthread -lpng -lz -lfreetype -lasound
LD_FLAGS += -L$(LINUXLIBS_INSTALL_DIR)/lib


COMPILE.c = $(VERBOSE) $(CC) $(C_FLAGS) $(CPP_FLAGS) -c
#LINK.c = $(VERBOSE) $(CC) $(LD_FLAGS) -Wl,-rpath=/root/lib
LINK.c = $(VERBOSE) $(CC) -Wl,-rpath=/root/lib

SOURCES = $(wildcard *.c)
# $(wildcard ../*.c)
HEADERS = $(wildcard *.h)
#$(wildcard ../*.h)

OBJFILES = $(SOURCES:%.c=%.o)

.PHONY: clean install

all:	dm6467

dm6467:	dm6467_al

dm6467_al:	$(TARGET)

install:	$(if $(wildcard $(TARGET)), install_$(TARGET))

install_$(TARGET):
	@install -d $(EXEC_DIR)
	@install $(TARGET) $(EXEC_DIR)
	@install $(TARGET).txt $(EXEC_DIR)
	@echo Installed $(TARGET) binaries to $(EXEC_DIR)..

$(TARGET):	$(OBJFILES) $(XDC_LFILE)
	@echo
	@echo Linking $@ from $^..
	$(LINK.c) -o $@ $^ $(NEW_HY_LIB_PATH) $(LD_FLAGS)

$(OBJFILES):	%.o: %.c $(HEADERS) $(XDC_CFLAGS)
	@echo Compiling $@ from $<..
	$(COMPILE.c) $(shell cat $(XDC_CFLAGS)) -o $@ $<

$(XDC_LFILE) $(XDC_CFLAGS):	$(XDC_CFGFILE)
	@echo
	@echo ======== Building $(TARGET) ========
	@echo Configuring application using $<
	@echo
	$(VERBOSE) PLATFORM_XDC="$(PLATFORM_XDC)" XDCPATH="$(XDC_PATH)" $(CONFIGURO) -c $(MVTOOL_DIR) -o $(XDC_CFG) -t $(PLATFORM_XDC_TARGET) -p $(PLATFORM_XDC) -b $(DMAI_INSTALL_DIR)/packages/config.bld $(XDC_CFGFILE)

clean:
	@echo Removing generated files..
	$(VERBOSE) -$(RM) -rf $(XDC_CFG) $(OBJFILES) $(TARGET) *~ *.d .dep
