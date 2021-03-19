#!/bin/bash

VER=2021.03.17

# elŝuti la fontoj :
if [ ! -d live ]
then
  if [ ! -f live.${VER}.tar.gz ]
  then
    wget http://www.live555.com/liveMedia/public/live.${VER}.tar.gz
  fi
  tar zxf live.${VER}.tar.gz
fi

PATH=$PWD/../toolchain/bin:$PATH
BASEROOT=$PWD/..
export HOST=mips-linux-gnu

cd live

echo "CROSS_COMPILE=${HOST}-"   >config.T21
echo "BASEROOT=${BASEROOT}"    >>config.T21

cat >>config.T21 <<'EOF'

COMPILE_OPTS =    -muclibc -mel $(INCLUDES) -I$(BASEROOT)/include -I. -O3 -DSOCKLEN_T=socklen_t -D_LARGEFILE_SOURCE=1 -D_FILE_OFFSET_BITS=64 -DXLOCALE_NOT_USED=1 -DLOCALE_NOT_USED -fPIC -DBS1 -DALLOW_RTSP_SERVER_PORT_REUSE=1
C =            c
C_COMPILER =        $(CROSS_COMPILE)gcc
CFLAGS +=        $(COMPILE_OPTS)
C_FLAGS = $(CFLAGS)
CPP =            cpp
CPLUSPLUS_COMPILER =    $(CROSS_COMPILE)g++
CPLUSPLUS_FLAGS =    $(COMPILE_OPTS) -Wall -DBSD=1
CPLUSPLUS_FLAGS += $(CPPFLAGS) -fexceptions
OBJ =            o
LINK =            $(CROSS_COMPILE)g++ -o
LINK_OPTS =  -muclibc  -mel -L. -L$(BASEROOT)/lib $(LDFLAGS)
CONSOLE_LINK_OPTS =    $(LINK_OPTS)
LIBRARY_LINK =        $(CROSS_COMPILE)gcc -o
LIBRARY_LINK_OPTS = -pthread -muclibc -shared -Wl,-soname,$(NAME).$(SHORT_LIB_SUFFIX) $(LDFLAGS)
LIB_SUFFIX =            so
SHORT_LIB_SUFFIX =            so
LIBS_FOR_CONSOLE_APPLICATION = $(CXXLIBS) -lssl -lcrypto
LIBS_FOR_GUI_APPLICATION = $(LIBS_FOR_CONSOLE_APPLICATION)
EXE =

DESTDIR=$(PWD)/../../../
PREFIX=
EOF

./genMakefiles T21
make
make install

# cd ..
# rm -rf live live.${VER}.tar.gz
