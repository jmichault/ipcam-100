#!/bin/bash


# elŝuti la fontoj :
if [ ! -d lighttpd1.4 ]
then
  git clone https://github.com/lighttpd/lighttpd1.4.git
fi
if [ ! -d lighttpd-mod_authn_tkt ]
then
  git clone https://github.com/gstrauss/lighttpd-mod_authn_tkt.git
fi

if [ ! -f lighttpd1.4/src/mod_authn_tkt.c ]
then
  cp -p lighttpd-mod_authn_tkt/mod_authn_tkt.c lighttpd1.4/src
fi
if [ ! -f lighttpd1.4/src/Makefile.am.patch ]
then
  cp -p lighttpd-mod_authn_tkt/Makefile.am.patch lighttpd1.4/src
fi
if ! grep "  mod_authn_tkt.c " lighttpd1.4/src/Makefile.am.patch 
then
  cat >>lighttpd1.4/src/Makefile.am.patch <<EOF
@@ -502,6 +502,7 @@
   mod_alias.c \\
   mod_auth.c \\
   mod_authn_file.c \\
+  mod_authn_tkt.c \\
   mod_cgi.c \\
   mod_deflate.c \\
   mod_dirlisting.c \\
EOF
fi
if ! grep "mod_authn_tkt.c" lighttpd1.4/src/Makefile.am
then
  cd lighttpd1.4/src
  patch Makefile.am Makefile.am.patch
  cd ../..
fi

# 
cd lighttpd1.4/

PATH=$PWD/../../toolchain/bin:$PATH
export HOST=mips-linux-gnu
export CFLAGS="-muclibc -mel"
export LDFLAGS="-muclibc -mel -L$PWD/../../lib"

export PCREPATH=$PWD/../..
export SSLPATH=$PWD/../..
export PCRECONFIG=$PCREPATH/bin/pcre-config
export PCRE_LIB=$PCREPATH/lib/libpcre.a
export CFLAGS="$CFLAGS -DHAVE_PCRE_H=1 -DHAVE_LIBPCRE=1 -I$PCREPATH/include"
export CPPFLAGS="$CFLAGS -DLIGHTTPD_STATIC -DHAVE_PCRE_H=1 -DHAVE_LIBPCRE=1 -I$PCREPATH/include"

if [ ! -f configure ]
then
  ./autogen.sh
fi

if [ ! -f Makefile ]
then
LIGHTTPD_STATIC=yes ./configure --prefix= --host=mips-linux-gnu --without-mysql --without-zlib --without-bzip2 --disable-ipv6 --enable-static --disable-shared --with-openssl --with-pcre
fi

if [ ! -f src/plugin-static.h ]
then
cat >src/plugin-static.h  <<'EOF'
PLUGIN_INIT(mod_alias)
PLUGIN_INIT(mod_auth)
PLUGIN_INIT(mod_authn_file)
PLUGIN_INIT(mod_authn_tkt)
PLUGIN_INIT(mod_redirect)
PLUGIN_INIT(mod_rewrite)
PLUGIN_INIT(mod_cgi)
PLUGIN_INIT(mod_fastcgi)
PLUGIN_INIT(mod_scgi)
PLUGIN_INIT(mod_ssi)
PLUGIN_INIT(mod_proxy)
PLUGIN_INIT(mod_indexfile)
PLUGIN_INIT(mod_dirlisting)
PLUGIN_INIT(mod_staticfile)
PLUGIN_INIT(mod_accesslog)
PLUGIN_INIT(mod_openssl)
PLUGIN_INIT(mod_setenv)
PLUGIN_INIT(mod_access)
PLUGIN_INIT(mod_deflate)
PLUGIN_INIT(mod_evasive)
PLUGIN_INIT(mod_expire)
PLUGIN_INIT(mod_extforward)
PLUGIN_INIT(mod_simple_vhost)
PLUGIN_INIT(mod_status)
PLUGIN_INIT(mod_userdir)
PLUGIN_INIT(mod_uploadprogress)
PLUGIN_INIT(mod_usertrack)
PLUGIN_INIT(mod_vhostdb)
EOF
fi

make 

DESTDIR=$PWD/../.. make install

#  cd ..
#  rm -rf lighttpd1.4/


