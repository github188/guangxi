#
# loadmodules configuring CMEM for DVSDK demos on DM6467 platform
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
#!/bin/sh

# Default Memory Map
#
# Start Addr    Size    Description
# -------------------------------------------
# 0x80000000     76 MB  Linux
# 0x84C00000    110 MB  CMEM
# 0x8ba00000     70 MB  CODEC SERVER

# Sample bootargs is given below
# mem=76M console=ttyS0,115200n8 root=/dev/nfs rw nfsroot=<nfsroot> ip=dhcp vpif_display.ch2_numbuffers=0 vpif_display.ch3_numbuffers=0 

rmmod cmemk.ko 2>/dev/null
rmmod dsplinkk.ko 2>/dev/null 

#insmod cmemk.ko phys_start=0x84C00000 phys_end=0x8ba00000 #pools=2x921600,1x460800,1x1048576,1x345600,2x86400,11x564528,5x677376,14x5396480,3x4147200,4x1451520,4x1843200
insmod cmemk.ko phys_start=0x87800000 phys_end=0x8ba00000 pools=2x921600,1x460800,1x1048576,1x345600,2x86400,11x564528,5x677376,4x5396480,3x4147200,4x1451520,4x1843200

insmod dsplinkk.ko

# alter dma queue mapping for better visual performance
if [ -f mapdmaq-hd ]
then
    ./mapdmaq-hd
fi

# make /dev/dsplink
rm -f /dev/dsplink
mknod /dev/dsplink c `awk "\\$2==\"dsplink\" {print \\$1}" /proc/devices` 0

