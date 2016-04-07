ifndef $(HYDIR)
	HYDIR = $(shell /bin/pwd)
endif



DAVINCI_INSTALL_DIR		= /opt
CMEM_INSTALL_DIR		= $(DAVINCI_INSTALL_DIR)/dvsdk_1_40_02_33/cmem_2_10/packages/ti/sdo/linuxutils/cmem
MVL_INSTALL_DIR			= $(DAVINCI_INSTALL_DIR)/mv_pro_4.0.1
MVL_KERNEL_INSTALL_DIR	= $(MVL_INSTALL_DIR)/montavista/pro/devkit/lsp/ti-davinci/linux-2.6.10_mvl401_LSP_01_30_00_082


PLATFORM ?= dm6467


ifeq ($(PLATFORM), dm6467)
	PREFIX = arm-none-linux-gnueabi-
    HOSTBUILD = arm-linux
else
    PREFIX            =
    HOSTBUILD         = i686-pc-linux
	HYENCLINKNAME     =
	HYENCAPILINKNAME  =
endif

HY3RDDIRNAME      =   3rd
HYBASEDIRNAME     =   base
HY4GDIRNAME       =   4g
HYCONFIGDIRNAME   =   config
HYIODIRNAME       =   io
HYPTZDIRNAME      =   ptz


HY3RDPATH         = $(HYDIR)/$(HY3RDDIRNAME)
HY3RDINC          = -I$(HY3RDPATH)/inc
HY3RDLIBPATH      = $(HY3RDPATH)/lib

HYBASEPATH = $(HYDIR)/$(HYBASEDIRNAME)
HYBASEINC  = -I$(HYBASEPATH)/inc \
             $(HY3RDINC)
HYBASELIBNAME = libhy_base.a

HY4GPATH = $(HYDIR)/$(HY4GDIRNAME)
HY4GINC  = -I$(HY4GPATH)/inc

HYCONFIGPATH = $(HYDIR)/$(HYCONFIGDIRNAME)
HYCONFIGINC  = -I$(HYCONFIGPATH)/inc
HYCONFIGLIBNAME = libhy_config.a

HYIOPATH = $(HYDIR)/$(HYIODIRNAME)
HYIOINC  = -I$(HYIOPATH)/inc
HYIOLIBNAME = libhy_io.a


HYPTZPATH = $(HYDIR)/$(HYPTZDIRNAME)
HYPTZINC  = -I$(HYPTZPATH)/inc \
            -I$(HYPTZPATH)/inc/onvif \
            -I$(HYPTZPATH)/inc/ffmpeg
HYPTZLINKNAME = -lssl -lcrypto -luuid -lpthread -lavformat -lavcodec -lavutil -lswresample -lov_ptz
HYPTZLINKPATH = -L$(HYPTZPATH)/lib \
                -L$(HYPTZPATH)/lib/ffmpeg \
                -L$(HYPTZPATH)/lib/openssl \
                -L$(HYPTZPATH)/lib/uuid
HYPTZLIBNAME = libhy_ptz.a


#used to store lib geneated when compilering
HYLIBPATH = $(HYDIR)/lib


CC=$(PREFIX)gcc
CXX =$(PREFIX)g++ -lpthread
AR=$(PREFIX)ar
RANLIB=$(PREFIX)ranlib
LINK=$(PREFIX)ld
STRIP = $(PREFIX)strip


CFLAGS = -Wall -fpic -Wno-unused-parameter -Wno-unused-function -Wno-unused-variable -Wwrite-strings -lstdc++
ifeq ($(PLATFORM), dm6467)
    CFLAGS+= -O3 -fno-strict-aliasing -D_ARM_APP_ -DWITH_OPENSSL -DWITH_DOM -D_REENTRANT -D_GNU_SOURCE=1 -pthread
endif

CPPFLAGS = -Wall -fpic -Wno-unused-parameter -Wno-unused-function -Wno-unused-variable -Wwrite-strings
ifeq ($(PLATFORM), dm6467)
#    CPPFLAGS+= $(INCLUDE_DIR)
endif










