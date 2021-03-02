NAME = snx_rtsp_server

PWD     	:= $(shell pwd)
BASE := ../..
TARGET      ?= ../../toolchain/bin/mips-linux-gnu
CROSS_COMPILE   ?= $(TARGET)-
CPP     	:= $(CROSS_COMPILE)g++
CC      	:= $(CROSS_COMPILE)g++
AR      	:= $(CROSS_COMPILE)ar
RANLIB      := $(CROSS_COMPILE)ranlib
STRIP 		:= $(CROSS_COMPILE)strip
INSTALL_DIR	?= ../rootfs/usr
BIN_INS_DIR	:= $(INSTALL_DIR)/bin/example
LIB_INS_DIR	:= $(INSTALL_DIR)/lib/example
CATEGORY	:= ipc_func/rtsp_server

INC     	= -I$(BASE)/include
INC 		+= -I$(PWD)/include

## toolchain
EXTRA_LDFLAGS	= -std=c++11$ 
EXTRA_CFLAGS	+= -I$(AUTOCONF_DIR) 

CFLAGS      += -Wall -I. $(INC) -DSOCKLEN_T=socklen_t -DNO_SSTREAM=1 -D_LARGEFILE_SOURCE=1  -D_FILE_OFFSET_BITS=64
#CFLAGS		+= -DBSD=1 -DLOCALE_NOT_USED
#CFLAGS		+= -DAUDIO_STREAM=1
#CFLAGS		+= -DDEBUG
CFLAGS		+= -std=c++11 -g

LDFLAGS     = -L../../lib -muclibc -lpthread -lm -lrt -lliveMedia -lgroupsock -lUsageEnvironment -lBasicUsageEnvironment -limp -lalog -g

SRC_OBJ	= main.o 
SRC_OBJ	+=ServerMediaSubsession.o
SRC_OBJ	+=H264_V4l2DeviceSource.o V4l2DeviceSource.o
SRC_OBJ	+=AlsaDeviceSource.o
SRC_OBJ	+=ImpJpegVideoDeviceSource.o
SRC_OBJ	+=JpegFrameParser.o
SRC_OBJ	+=ImpEncoder.o
SRC_OBJ +=../v4l2rtspserver-tools/sharedmem.o

#targets =
targets = rtsp_server


.PHONY : clean distclean all
%.o : %.cpp
	$(CPP) -c $(CFLAGS) $< -o $@

%.o : %.c
	$(CC) -c $(CFLAGS) $<

all: $(targets)


rtsp_server:  $(SRC_OBJ)
	$(CPP) -o$@  $(LDFLAGS)  $^  
#	$(STRIP) $@


.PHONY: install
install:
	@ if [ ! -d $(BIN_INS_DIR)/$(CATEGORY) ]; \
	then \
	install -d $(BIN_INS_DIR)/$(CATEGORY); \
	fi
	install -c $(NAME) $(BIN_INS_DIR)/$(CATEGORY)

clean:
	rm -f *.o *.yuv *.bak *.a *.out *.so snx_rtsp_server
	#$(MAKE) -C live $@  INSTALL_DIR=$(abspath $(BIN_INS_DIR))

distclean : clean
	rm -f $(targets)
