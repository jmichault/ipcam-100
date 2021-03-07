#!/usr/bin/env bash
TOOLCHAIN=$(pwd)/../../toolchain/bin
CROSS_COMPILE=$TOOLCHAIN/mips-linux-gnu-
export CROSS_COMPILE=${CROSS_COMPILE}
export CC=${CROSS_COMPILE}gcc
export LD=${CROSS_COMPILE}ld
export CFLAGS="-g -muclibc "
export CPPFLAGS="-g -muclibc "
export LDFLAGS="-g -muclibc "
make clean
make all
ftp-upload -h ipcam2 -u root --password jco66688 -d /opt/media/mmcblk0p1/bin rtsp_server 
