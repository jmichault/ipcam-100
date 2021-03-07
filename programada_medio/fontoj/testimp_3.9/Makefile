CROSS_COMPILE=        ../../toolchain/bin/mips-linux-gnu-
CC = $(CROSS_COMPILE)gcc
CPLUSPLUS = $(CROSS_COMPILE)g++
CPP = $(CROSS_COMPILE)g++
LD = $(CROSS_COMPILE)ld
AR = $(CROSS_COMPILE)ar cr
STRIP = $(CROSS_COMPILE)strip
CFLAGS = $(INCLUDES) -I../../include -O2 -Wall -march=mips32r2 -std=gnu99
CFLAGS += -muclibc -g
CPPFLAGS = $(INCLUDES) -I../../include -O2 -Wall -muclibc
LDFLAGS += -muclibc -limp -lpthread -lm -lrt -L../../lib -lalog
LDFLAGS += -Wl,-gc-sections -g


all: 	testimp sample-Encoder-jpeg sample-Encoder-h264 getsnap test

lit_infrarouge: lit_infrarouge.o
	$(CPLUSPLUS) $(LDFLAGS) -o $@ $^ $(LIBS) -lsysutils

test: test.o
	$(CPLUSPLUS) $(LDFLAGS) -o $@ $^ $(LIBS)

#motor: motor.o
#	$(CPLUSPLUS) $(LDFLAGS) -o $@ $^ $(LIBS) -lpthread -lm -lrt
#	$(STRIP) $@

sample-Encoder-h264: sample-common.o sample-Encoder-h264.o
	$(CPLUSPLUS) $(LDFLAGS) -o $@ $^ $(LIBS)
#	$(STRIP) $@

sample-Audio: sample-common.o sample-Audio.o
	$(CPLUSPLUS) $(LDFLAGS) -o $@ $^ $(LIBS)
#	$(STRIP) $@

getsnap: sample-common.o getsnap.o
	$(CPLUSPLUS) $(LDFLAGS) -o $@ $^ $(LIBS) 
#	$(STRIP) $@

sample-Encoder-jpeg: sample-common.o sample-Encoder-jpeg.o
	$(CPLUSPLUS) $(LDFLAGS) -o $@ $^ $(LIBS) 
#	$(STRIP) $@



%.o:%.c sample-common.h
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f *.o *~

distclean: clean
	rm -f $(SAMPLES)
