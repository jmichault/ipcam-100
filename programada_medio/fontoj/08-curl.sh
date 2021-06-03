#!/bin/bash


# elŝuti la fontoj :
if [ ! -d curl ]
then
  git clone https://github.com/curl/curl.git
fi

# 
PATH=$PWD/../toolchain/bin:$PATH
export HOST=mips-linux-gnu
export CFLAGS="-muclibc -mel -I$(dirname $PWD)/include"
export CXXFLAGS="-muclibc -mel -I$(dirname $PWD)/include"
export LDFLAGS="-muclibc -mel -L$(dirname $PWD)/lib"
export CROSS_COMPILE=$PWD/../toolchain/bin/mips-linux-gnu-


cd curl
autoreconf -fi
./configure --host=mips-linux-gnu --with-openssl
make


#cp -p curl ../../../karto/bin/

