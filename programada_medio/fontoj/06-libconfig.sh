#!/bin/bash

VER=1.7.2


# elŝuti la fontoj :
if [ ! -d libconfig-${VER} ]
then
  if [ ! -f libconfig-${VER}.tar.gz ]
  then
    wget https://hyperrealm.github.io/libconfig/dist/libconfig-${VER}.tar.gz
  fi
  tar zxf libconfig-${VER}.tar.gz
fi

# 
PATH=$PWD/../toolchain/bin:$PATH
export HOST=mips-linux-gnu
export C_INCLUDE_PATH="$PWD/../include"
export LIBRARY_PATH="$PWD/../lib"
echo C_INCLUDE_PATH=$C_INCLUDE_PATH
export CFLAGS="-muclibc -mel"
export LDFLAGS="-muclibc -mel"

cd libconfig-${VER}

if [ ! -f Makefile ]
then
  ./configure --host=mips-linux-gnu --prefix= --disable-cxx --disable-examples
fi

make
make install
DESTDIR=$PWD/../.. make install

# konstrui ls-config :
cd contrib/ls-config/src
echo "#define gettext(x) x
#define bindtextdomain(x,y) {}
#define textdomain(x) {}" >libintl.h
${HOST}-gcc ${CFLAGS} -I. -o ls-config ls-config.c -L../lib -lm -lconfig -L$LIBRARY_PATH
cp -p ls-config $LIBRARY_PATH/../bin


#  cd ..
#  rm -rf libconfig-${VER}


