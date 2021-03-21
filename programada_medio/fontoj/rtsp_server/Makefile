NAME = snx_rtsp_server

PWD     	:= $(shell pwd)
BASE := ../..
TARGET      ?= ../../toolchain/bin/mips-linux-gnu
CROSS_COMPILE   ?= $(TARGET)-
CPP     	:= $(CROSS_COMPILE)g++
CC      	:= $(CROSS_COMPILE)gcc
AR      	:= $(CROSS_COMPILE)ar
RANLIB      := $(CROSS_COMPILE)ranlib
STRIP 		:= $(CROSS_COMPILE)strip
INSTALL_DIR	?= ../rootfs/usr
BIN_INS_DIR	:= $(INSTALL_DIR)/bin/example
LIB_INS_DIR	:= $(INSTALL_DIR)/lib/example
CATEGORY	:= ipc_func/rtsp_server

INC     	= -I$(BASE)/include
INC     	+= -I$(BASE)/include/liveMedia
INC     	+= -I$(BASE)/include/groupsock
INC     	+= -I$(BASE)/include/UsageEnvironment
INC 		+= -I$(PWD)/include

## toolchain
EXTRA_LDFLAGS	= -std=c++11 
EXTRA_CFLAGS	+= -I$(AUTOCONF_DIR) 

CFLAGS      += -Wall -I. $(INC) -DSOCKLEN_T=socklen_t -DNO_SSTREAM=1 -D_LARGEFILE_SOURCE=1  -D_FILE_OFFSET_BITS=64
#CFLAGS		+= -DBSD=1 -DLOCALE_NOT_USED
#CFLAGS		+= -DAUDIO_STREAM=1
#CFLAGS		+= -DDEBUG
CFLAGS		+= -g

LDFLAGS     = -L../../lib -muclibc -lpthread -lm -lrt -g
LDFLAGS += -lliveMedia -lgroupsock -lUsageEnvironment -lBasicUsageEnvironment
LDFLAGS += -limp -lalog 
LDFLAGS += -lssl -ltls -lcrypto

SRC_OBJ	= main.o 
SRC_OBJ	+=ServerMediaSubsession.o
SRC_OBJ	+=ImpJpegVideoDeviceSource.o
SRC_OBJ	+=JpegFrameParser.o
SRC_OBJ	+=imp_komuna.o imp_vars_h264.o
SRC_OBJ +=../rtspserver-tools/sharedmem.o

#targets =
targets = rtsp_server


.PHONY : clean distclean all
%.o : %.cpp
	$(CPP) -c $(CFLAGS) -std=c++11 $< -o $@

%.o : %.c
	$(CC) -c $(CFLAGS) -std=c99 $<

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
