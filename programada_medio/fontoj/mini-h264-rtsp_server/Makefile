CROSS_COMPILE=    ../../toolchain/bin/mips-linux-gnu-
CC = $(CROSS_COMPILE)gcc
CPLUSPLUS = $(CROSS_COMPILE)g++
CPP = $(CROSS_COMPILE)g++
LD = $(CROSS_COMPILE)ld
AR = $(CROSS_COMPILE)ar cr
STRIP = $(CROSS_COMPILE)strip
CFLAGS = $(INCLUDES) -I include -I../../include -O2 -Wall -march=mips32r2 -std=gnu99
CFLAGS += -muclibc -g
CPPFLAGS = $(INCLUDES) -I include -I../../include -O2 -Wall -muclibc -std=c++11 -g
LDFLAGS += -muclibc -limp -lpthread -lm -lrt -L../../lib -lalog
LDFLAGS += -Wl,-gc-sections -g
LDFLAGS += -lliveMedia -lgroupsock -lBasicUsageEnvironment -lUsageEnvironment


all: 	rtsp_server

rtsp_server: DynamicRTSPServer.o main.o H264ImpServerMediaSubsession.o imp_komuna.o imp_vars_h264.o ImpSource.o
	$(CPLUSPLUS) $(LDFLAGS) -o $@ $^ $(LIBS)
#	$(STRIP) $@


%.o:%.c 
	$(CC) -c $(CFLAGS) $< -o $@

%.o:%.cpp 
	$(CPP) -c $(CPPFLAGS) $< -o $@

clean:
	rm -f *.o *~

