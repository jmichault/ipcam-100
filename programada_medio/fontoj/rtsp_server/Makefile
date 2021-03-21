CROSS_COMPILE=    ../../toolchain/bin/mips-linux-gnu-
CC = $(CROSS_COMPILE)gcc
CPLUSPLUS = $(CROSS_COMPILE)g++
CPP = $(CROSS_COMPILE)g++
LD = $(CROSS_COMPILE)ld
AR = $(CROSS_COMPILE)ar cr
STRIP = $(CROSS_COMPILE)strip
INCLUDES += -I include -I../../include
INCLUDES += -I../../include/liveMedia
INCLUDES += -I../../include/groupsock
INCLUDES += -I../../include/UsageEnvironment
CFLAGS = $(INCLUDES) -O2 -Wall -march=mips32r2 -std=gnu99
CFLAGS += -muclibc -g
CPPFLAGS = $(INCLUDES) -I include -I../../include -O2 -Wall -muclibc -std=c++11 -g
LDFLAGS += -muclibc -limp -lpthread -lm -lrt -L../../lib -lalog
LDFLAGS += -Wl,-gc-sections -g
LDFLAGS += -lssl -ltls -lcrypto -lliveMedia -lgroupsock -lBasicUsageEnvironment -lUsageEnvironment


all: 	rtsp_server

# RTSP Objs :
OBJS = DynamicRTSPServer.o main.o
# T21 Objs
OBJS += imp_komuna.o imp_vars_h264.o
# H264 Objs :
OBJS += H264ImpServerMediaSubsession.o ImpSource.o
# Mjpeg Objs :
OBJS += ImpJpegVideoDeviceSource.o JpegFrameParser.o ServerMediaSubsession.o
OBJS += ../rtspserver-tools/sharedmem.o

rtsp_server: $(OBJS)
	$(CPLUSPLUS) $(LDFLAGS) -o $@ $^ $(LIBS)
#	$(STRIP) $@


%.o:%.c 
	$(CC) -c $(CFLAGS) $< -o $@

%.o:%.cpp 
	$(CPP) -c $(CPPFLAGS) $< -o $@

clean:
	rm -f *.o *~

