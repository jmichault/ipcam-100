#!/bin/sh

ifconfig eth0 down
ifconfig wlan0 down
ps | awk '/[t]elnetd/ {print $1}' | xargs kill -9 &>/dev/null
## stop_cloud :
ps | awk '/[a]uto_run.sh/ {print $1}' | while read PID; do kill -9 $PID; done;
ps | awk '/[j]co_server/ {print $1}' | xargs kill -9 &>/dev/null
echo 'V'>/dev/watchdog
echo 'V'>/dev/watchdog0
rm "${SDCARD}/cid.txt" &>/dev/null


export SDCARD="/opt/media/mmcblk0p1"
export LD_LIBRARY_PATH="${SDCARD}/lib:/lib/:/ipc/lib/"

export CONFIGPATH="${SDCARD}/config"
export LOGDIR="${SDCARD}/log"
export LOGPATH="$LOGDIR/startup.log"

## Load some common functions:
. ${SDCARD}/scripts/common_functions.sh

# init ir_led gpio :
init_gpio 46
# init blue_led gpio :
init_gpio 81
# init motor :
. ${SDCARD}/config/ptz.conf
(${SDCARD}/bin/motor -p;${SDCARD}/bin/motor -x $X0 -y $Y0 -r $RAPIDECO) &

## Create root user home directory and etc directory on sdcard:
if [ ! -d /opt/root ]; then
  mkdir /opt/root
  echo "Created root user home directory" >> $LOGPATH
fi
mkdir -p ${SDCARD}/etc
if [ ! -f "${SDCARD}/etc/profile" ]
then
    cp -fRL "/etc/profile" ${SDCARD}/etc
    echo "
export SDCARD=/opt/media/mmcblk0p1
export PATH=${SDCARD}/bin:$PATH
export LD_LIBRARY_PATH=\"${SDCARD}/lib:/lib/:/ipc/lib/\"
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

if [ ! -d /opt/root ]; then mkdir /opt/root; fi
mount -o bind ${SDCARD}/bin/busybox /bin/busybox
echo "Bind mounted ${SDCARD}/bin/busybox to /bin/busybox" >> $LOGPATH
mount -o bind /opt/root /root
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

# krei linkoj por ssh kaj scp
if [ ! -d /opt/bin ] ; then mkdir /opt/bin; fi
for x in ssh scp dbclient ; do
  if [ ! -L /opt/bin/$x ] ; then ln -s ${SDCARD}/bin/dropbearmulti /opt/bin/$x ; fi
done

## Set Hostname
if [ ! -f $CONFIGPATH/hostname.conf ]; then
  cp $CONFIGPATH/hostname.conf.dist $CONFIGPATH/hostname.conf
fi
hostname -F $CONFIGPATH/hostname.conf

## start network
${SDCARD}/controlscripts/network

## Set Timezone
set_timezone

## Sync the time via NTP:
if [ ! -f $CONFIGPATH/ntp_srv.conf ]; then
  cp $CONFIGPATH/ntp_srv.conf.dist $CONFIGPATH/ntp_srv.conf
fi
ntp_srv="$(cat "$CONFIGPATH/ntp_srv.conf")"
timeout -t 30 sh -c "until ping -c1 \"$ntp_srv\" &>/dev/null; do sleep 3; done";
${SDCARD}/bin/busybox ntpd -p "$ntp_srv"

## retservilo estas deviga
${SDCARD}/config/autostart/system-webserver

## Autostart all enabled services:
for i in ${SDCARD}/config/autostart/*; do
  $i &
done

## Autostart startup userscripts
for i in ${SDCARD}/config/userscripts/startup/*; do
  $i &
done

