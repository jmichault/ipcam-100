#!/bin/bash

VER=3.2.5


# elŝuti la fontoj :
if [ ! -d portable-${VER} ]
then
  if [ ! -f v${VER}.tar.gz ]
  then
    wget https://github.com/libressl-portable/portable/archive/v${VER}.tar.gz
  fi
  tar zxf v${VER}.tar.gz
fi

# 
PATH=$PWD/../toolchain/bin:$PATH
export HOST=mips-linux-gnu
export CFLAGS="-muclibc -mel"
export LDFLAGS="-muclibc -mel"

cd portable-${VER}

# por «'program_invocation_short_name' undeclared »
sed -i 's/program_invocation_short_name/"?"/g' crypto/compat/getprogname_linux.c


if [ ! -f configure ]
then
  ./autogen.sh
fi
if [ ! -f Makefile ]
then
  CC=${HOST}-gcc ./configure --host=${HOST} --prefix= --with-pic
fi

make -j4

DESTDIR=$PWD/../.. make install

#  cd ..
#  rm -rf portable-${VER} v${VER}.tar.gz


# Rezulto :
# ../../lib/libcrypto*
# ../../lib/libssl*
# ../../lib/libtls*
# ../../include/openssl/*
# ../../include/tls.h
# ../../etc/ssl
