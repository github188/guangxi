#
# Makefile for making the demos on DM6467 platform
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

SUBDIRS = $(sort $(dir $(wildcard */)))
#SUBDIRS += hy_proj

CLEANSUBDIRS = $(addsuffix .clean, $(SUBDIRS))
INSTALLSUBDIRS = $(addsuffix .install, $(SUBDIRS))

ifeq ($(VERBOSE), true)
	MAKE := $(MAKE)
else
	MAKE := $(MAKE) --no-print-directory
endif

.PHONY: install clean $(SUBDIRS) $(INSTALLSUBDIRS) $(CLEANSUBDIRS)   hy_proj decode  interface

all:	hy_proj decode 
#interface

hy_proj:
	@echo
	@echo Making all in subdirectory $@...
	@$(MAKE) -C hy_proj

encode:
	@echo
	@echo Making all in subdirectory $@...
	@$(MAKE) -C encode

decode:
	@echo
	@echo Making all in subdirectory $@...
	@$(MAKE) -C decode


interface:
	@echo
	@echo Making all in subdirectory $@...
	@$(MAKE) -C interface

install: $(INSTALLSUBDIRS)
	@echo
	@install -d $(EXEC_DIR)
	@make -C ../data install
	@install -d $(EXEC_DIR)/thirdpartydemos
	@install loadmodules.sh $(EXEC_DIR)

$(INSTALLSUBDIRS):
	@echo
	@echo Executing make install in subdirectory $(basename $@)...
	@cd $(basename $@) ; $(MAKE) install

clean:	$(CLEANSUBDIRS)

$(CLEANSUBDIRS):
	@cd $(basename $@) ; $(MAKE) clean
