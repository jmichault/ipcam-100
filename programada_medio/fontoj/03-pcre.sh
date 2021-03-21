#!/bin/bash

VER=8.43


# elŝuti la fontoj :
if [ ! -d pcre-${VER} ]
then
  if [ ! -f pcre-${VER}.tar.gz ]
  then
    wget https://ftp.pcre.org/pub/pcre/pcre-${VER}.tar.gz
  fi
  tar zxf pcre-${VER}.tar.gz
fi

# 
PATH=$PWD/../toolchain/bin:$PATH
export HOST=mips-linux-gnu
export CFLAGS="-muclibc -mel"
export CXXFLAGS="-muclibc -mel"
export LDFLAGS="-muclibc -mel"

cd pcre-${VER}

if [ ! -f Makefile ]
then
  ./configure --host=mips-linux-gnu --prefix=
fi

make
make install

DESTDIR=$PWD/../.. make install

#  cd ..
#  rm -rf pcre-${VER}


