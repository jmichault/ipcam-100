#!/bin/bash


# elŝuti la fontoj :
if [ ! -d busybox ]
then
  git clone --depth=1  git://git.busybox.net/busybox
fi

# 
PATH=$PWD/../toolchain/bin:$PATH
export HOST=mips-linux-gnu
export CFLAGS="-muclibc -mel"
export CXXFLAGS="-muclibc -mel"
export LDFLAGS="-muclibc -mel"
export CROSS_COMPILE=$PWD/../toolchain/bin/mips-linux-gnu-

cd busybox
if [ ! -f Makefile ]
then
  make clean
  make CROSS_COMPILE=$CROSS_COMPILE defconfig
  sleep 1
fi

#Remove some packages that doesn't compile
sed -i 's/CONFIG_FALLOCATE=y/CONFIG_FALLOCATE=n/' .config
sed -i 's/CONFIG_NSENTER=y/CONFIG_NSENTER=n/' .config
sed -i 's/CONFIG_FSYNC=y/CONFIG_FSYNC=n/' .config
sed -i 's/CONFIG_FEATURE_SYNC_FANCY=y/CONFIG_FEATURE_SYNC_FANCY=n/' .config
sed -i 's/CONFIG_TRACEROUTE=y/CONFIG_TRACEROUTE=n/' .config
sed -i 's/CONFIG_TRACEROUTE6=y/CONFIG_TRACEROUTE6=n/' .config
sed -i 's/CONFIG_TRACEROUTE_VERBOSE=y/CONFIG_TRACEROUTE_VERBOSE=n/' .config
sed -i 's/CONFIG_TRACEROUTE_USE_ICMP=y/CONFIG_TRACEROUTE_USE_ICMP=n/' .config
# activate flash_eraseall
sed -i 's/# CONFIG_FLASH_ERASEALL is not set/CONFIG_FLASH_ERASEALL=y/' .config

make CROSS_COMPILE=$CROSS_COMPILE

#cp -p busybox ../../../karto/bin/

