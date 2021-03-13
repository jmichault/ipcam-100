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


all: 	demo-h264

demo-h264: imp_common.o imp_vars_h264.o demo-h264.o
	$(CPLUSPLUS) $(LDFLAGS) -o $@ $^ $(LIBS)
#	$(STRIP) $@


%.o:%.c sample-common.h
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f *.o *~

