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

## Create crontab dir and start crond:
if [ ! -d ${SDCARD}/config/cron ]; then
  mkdir -p ${CONFIGPATH}/cron/crontabs
  CRONPERIODIC="${CONFIGPATH}/cron/periodic"
  echo ${CONFIGPATH}/cron/crontabs/periodic
  # Wish busybox sh had brace expansion...
  mkdir -p ${CRONPERIODIC}/15min \
           ${CRONPERIODIC}/hourly \
           ${CRONPERIODIC}/daily \
           ${CRONPERIODIC}/weekly \
           ${CRONPERIODIC}/monthly
  cat > ${CONFIGPATH}/cron/crontabs/root <<EOF
# min   hour    day     month   weekday command
*/15    *       *       *       *       busybox run-parts ${CRONPERIODIC}/15min
0       *       *       *       *       busybox run-parts ${CRONPERIODIC}/hourly
0       2       *       *       *       busybox run-parts ${CRONPERIODIC}/daily
0       3       *       *       6       busybox run-parts ${CRONPERIODIC}/weekly
0       5       1       *       *       busybox run-parts ${CRONPERIODIC}/monthly
EOF
  echo "Created cron directories and standard interval jobs" >> $LOGPATH
fi
${SDCARD}/bin/busybox crond -L ${SDCARD}/log/crond.log -c ${SDCARD}/config/cron/crontabs

## Set Hostname
if [ ! -f $CONFIGPATH/hostname.conf ]; then
  cp $CONFIGPATH/hostname.conf.dist $CONFIGPATH/hostname.conf
fi
hostname -F $CONFIGPATH/hostname.conf

## Set Timezone
set_timezone

## Sync the time via NTP:
if [ ! -f $CONFIGPATH/ntp_srv.conf ]; then
  cp $CONFIGPATH/ntp_srv.conf.dist $CONFIGPATH/ntp_srv.conf
fi
ntp_srv="$(cat "$CONFIGPATH/ntp_srv.conf")"
timeout -t 30 sh -c "until ping -c1 \"$ntp_srv\" &>/dev/null; do sleep 3; done";
${SDCARD}/bin/busybox ntpd -p "$ntp_srv"

${SDCARD}/config/autostart/system-webserver

## Autostart all enabled services:
for i in ${SDCARD}/config/autostart/*; do
  $i &
done

## Autostart startup userscripts
for i in ${SDCARD}/config/userscripts/startup/*; do
  $i &
done

while :; do
  @${count:-0}                
  /opt/media/sdc/bin/rtsp_server -W 1280 -H 720 -j 60 -Q 5 -u media/stream1 -P 554
  let count++
  sleep 1
  [ "${count:-0}" -ge 5 ] && reboot -f
done 

