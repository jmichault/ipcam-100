#!/bin/sh

export SDCARD="/opt/media/mmcblk0p1"
export LD_LIBRARY_PATH="${SDCARD}/lib:/lib/:/ipc/lib/"
export CONFIGPATH="${SDCARD}/config"
export LOGDIR="${SDCARD}/log"
export LOGPATH="$LOGDIR/startup.log"

## Load some common functions:
. ${SDCARD}/scripts/common_functions.sh

## Create root user home directory and etc directory on sdcard:
if [ ! -d /opt/root ]; then
  mkdir /opt/root
  echo "Created root user home directory" >> $LOGPATH
fi
if [ ! -d ${SDCARD}/etc ]; then
 mkdir ${SDCARD}/etc
 cp -rp ${SDCARD}/etc.dist/* ${SDCARD}/etc
 while IFS= read -r etc_element
 do
  if [ ! -f "${SDCARD}/etc/$etc_element" ] && [ ! -d "${SDCARD}/etc/$etc_element" ]; then
    cp -fRL "/etc/$etc_element" ${SDCARD}/etc
  fi
done <<- END
TZ
protocols
fstab
inittab
init.d
hosts
group
resolv.conf
hostname
os-release
sensor
webrtc_profile.ini
END

  if [ ! -f "${SDCARD}/etc/profile" ]
  then
    cp -fRL "/etc/profile" ${SDCARD}/etc
    echo "
export SDCARD=/opt/media/mmcblk0p1
export PATH=${SDCARD}/bin:/opt/bin:$PATH
export LD_LIBRARY_PATH=\"${SDCARD}/lib:/lib/:/ipc/lib/\"
export CONFIGPATH=\"${SDCARD}/config\"
export LOGDIR=\"${SDCARD}/log\"
export LOGPATH=\"$LOGDIR/startup.log\"
" >>${SDCARD}/etc/profile
  fi
  echo "Created etc directory on sdcard" >> $LOGPATH
fi

# create config directories if needed
if [ ! -d $CONFIGPATH ]; then
  mkdir $CONFIGPATH
  cp -rp ${CONFIGPATH}.dist/* $CONFIGPATH
fi
if [ ! -d ${SDCARD}/www/ipcam/config ]; then
  mkdir ${SDCARD}/www/ipcam/config
  cp -rp ${SDCARD}/www/ipcam/config.dist/* ${SDCARD}/www/ipcam/config
fi

# krei linkoj por ssh , scp, ssl_client, ...
if [ ! -d /opt/bin ] ; then mkdir /opt/bin; fi
for x in ssh scp dbclient ssl_client ; do
  if [ ! -L /opt/bin/$x ] ; then ln -s ${SDCARD}/bin/dropbearmulti /opt/bin/$x ; fi
done
for x in bunzip2 bzip2 gzip gunzip ; do
  if [ ! -L /opt/bin/$x ] ; then ln -s ${SDCARD}/bin/busybox /opt/bin/$x ; fi
done

