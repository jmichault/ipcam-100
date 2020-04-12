#!/bin/sh
export SDCARD="/opt/media/mmcblk0p1"
export LD_LIBRARY_PATH="${SDCARD}/lib:/lib/:/ipc/lib/"

export CONFIGPATH="${SDCARD}/config"
export LOGDIR="${SDCARD}/log"
export LOGPATH="$LOGDIR/startup.log"

## Load some common functions:
. ${SDCARD}/scripts/common_functions.sh

# Mount bind to extended busybox.
#mount -o bind ${SDCARD}/bin/busybox /bin/busybox

sleep 15

## stop_cloud :
ps | awk '/[a]uto_run.sh/ {print $1}' | while read PID; do kill -9 $PID; done;
ps | awk '/[j]co_server/ {print $1}' | xargs kill -9 &>/dev/null
echo 'V'>/dev/watchdog
echo 'V'>/dev/watchdog0    
rm "${SDCARD}/cid.txt" &>/dev/null

#sleep 1
# init ir_led gpio :
init_gpio 46
# init blue_led gpio :
init_gpio 81

## Create root user home directory and etc directory on sdcard:
if [ ! -d ${SDCARD}/root ]; then
  mkdir ${SDCARD}/root
  echo 'PATH=${SDCARD}/bin:$PATH' > ${SDCARD}/root/.profile
  echo "Created root user home directory" >> $LOGPATH
fi
mkdir -p ${SDCARD}/etc
if [ ! -f "${SDCARD}/etc/profile" ]
then
    cp -fRL "/etc/profile" ${SDCARD}/etc
    echo "
export SDCARD=${SDCARD}
LD_LIBRARY_PATH=\"${SDCARD}/lib:\$LD_LIBRARY_PATH\"
export CONFIGPATH=\"${SDCARD}/config\"                                                            
export LOGDIR=\"${SDCARD}/log\"                                                              
export LOGPATH=\"$LOGDIR/startup.log\"          
" >>${SDCARD}/etc/profile
fi
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
echo "Created etc directory on sdcard" >> $LOGPATH

mount -o bind ${SDCARD}/bin/busybox /bin/busybox
echo "Bind mounted ${SDCARD}/bin/busybox to /bin/busybox" >> $LOGPATH
mount -o bind ${SDCARD}/root /root
echo "Bind mounted ${SDCARD}/root to /root" >> $LOGPATH
mount -o bind ${SDCARD}/etc /etc
echo "Bind mounted ${SDCARD}/etc to /etc" >> $LOGPATH


${SDCARD}/config/autostart/system-webserver

${SDCARD}/controlscripts/ftp-server start

#${SDCARD}/controlscripts/rtsp start
while :; do
  @${count:-0}                
  /opt/media/sdc/bin/rtsp_server -W 1280 -H 720 -j 60 -Q 5 -u media/stream1 -P 554
  let count++
  sleep 1
  [ "${count:-0}" -ge 5 ] && reboot -f
done 

