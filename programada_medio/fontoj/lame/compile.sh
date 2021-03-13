#!/usr/bin/env bash

TOOLCHAIN=$(pwd)/../toolchain/bin
CROSS_COMPILE=$TOOLCHAIN/mips-linux-gnu-

echo "Using Toolchain $TOOLCHAIN"

export CC=${CROSS_COMPILE}gcc
export CFLAGS="-muclibc -O2"
export CPPFLAGS="-muclibc -O2"
export LDFLAGS="-muclibc -O2"

make clean
./configure --host=mips-linux-gnu --prefix=${PWD}/_install
make libmp3lame.so
make install
