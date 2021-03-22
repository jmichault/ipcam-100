#!/bin/bash


# elŝuti la fontoj :
if [ ! -d dropbear ]
then
  git clone https://github.com/mkj/dropbear
fi

# 
cd dropbear/

if [ ! -f localoptions.h ]
then
cat >localoptions.h  <<'EOF'
#ifndef DROPBEAR_LOCAL_OPTIONS_H_
#define DROPBEAR_LOCAL_OPTIONS_H_

/* Default hostkey paths - these can be specified on the command line */
#define DSS_PRIV_FILENAME "/opt/media/mmcblk0p1/config/dropbear_dss_host_key"
#define RSA_PRIV_FILENAME "/opt/media/mmcblk0p1/config/dropbear_rsa_host_key"
#define ECDSA_PRIV_FILENAME "/opt/media/mmcblk0p1/config/dropbear_ecdsa_host_key"
#define ED25519_PRIV_FILENAME "/opt/media/mmcblk0p1/config/dropbear_ed25519_host_key"

/* The default path. This will often get replaced by the shell */
#define DEFAULT_PATH "/opt/bin:/opt/media/mmcblk0p1/bin:/ipc/bin:/appfs/bin:/usr/bin:/bin"

#endif /* DROPBEAR_LOCAL_OPTIONS_H_ */
EOF
fi

PATH=$PWD/../../toolchain/bin:$PATH
export HOST=mips-linux-gnu
export CFLAGS="-O3 -muclibc -mel"
export LDFLAGS="-muclibc -mel -L$PWD/../../lib"

export PCREPATH=$PWD/../..
export SSLPATH=$PWD/../..
export PCRECONFIG=$PCREPATH/bin/pcre-config
export PCRE_LIB=$PCREPATH/lib/libpcre.a
export CFLAGS="$CFLAGS -DHAVE_PCRE_H=1 -DHAVE_LIBPCRE=1 -I$PCREPATH/include"
export CPPFLAGS="$CFLAGS -DLIGHTTPD_STATIC -DHAVE_PCRE_H=1 -DHAVE_LIBPCRE=1 -I$PCREPATH/include"

if [ ! -f configure ]
then
  autoconf ; autoheader
fi

if [ ! -f Makefile ]
then
  ./configure --host=$HOST --disable-zlib --prefix=
fi

make PROGRAMS="dropbear dbclient scp dropbearkey dropbearconvert" MULTI=1 SCPPROGRESS=1

DESTDIR=$PWD/../.. make inst_dropbearmulti

#  cd ..
#  rm -rf dropbear/


