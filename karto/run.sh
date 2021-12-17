#!/bin/sh

export SDCARD="/opt/media/mmcblk0p1"

${SDCARD}/initconf.sh

. ${SDCARD}/etc/profile

# stop network
ifconfig eth0 down
ifconfig wlan0 down
ps | awk '/[t]elnetd/ {print $1}' | xargs kill -9 &>/dev/null
## stop_cloud :
ps | awk '/[a]uto_run.sh/ {print $1}' | while read PID; do kill -9 $PID; done;
ps | awk '/[j]co_server/ {print $1}' | xargs kill -9 &>/dev/null
echo 'V'>/dev/watchdog
echo 'V'>/dev/watchdog0
rm "${SDCARD}/cid.txt" &>/dev/null
# kill other daemons :
killall -9 hostapd
killall -9 udhcpd
killall -9 udhcpc

# agordi horloĝon je la dato de la lasta protokolo.
cd "${SDCARD}"
touch log/bootclock
hwclock -s
touch log/hwclock
FIC=$(ls -rt --color=never log|tail -n 1)
if [ x"$FIC" == xhwclock ]
then
  echo "agordi horloĝon de RTC"
  hwclock -s
elif [ x"$FIC" != xbootclock ]
then
  echo "agordi horloĝon de log/$FIC"
  LASTDATE=$(date -Iseconds -r log/"$FIC"|sed "s/[T:+-]/:/g
"|awk -F: '{print $2 $3 $4 $5 $1 "." $6;}')
  date $LASTDATE
  hwclock -uw
fi
#
echo "===============================" >>$LOGPATH
echo $(date -Iseconds) " Komencante. " >>$LOGPATH

# ĝisdatigo de firmvaro se necese
if [ -f "${SDCARD}/update.zip" ]
then
  echo $(date -Iseconds) " ĝisdatigo de «update.zip»".  >>$LOGPATH
  echo "========================" > log/update.log
  echo "ĝisdatigo per «update.zip»" >>log/update.log
  ls -l update.zip >>log/update.log
  echo "========================" >>log/update.log
  unzip -o update.zip >>log/update.log 2>&1
  rm update.zip >>log/update.log 2>&1
  sync >>log/update.log 2>&1
  echo "========================" >>log/update.log
  echo "ĝisdatigo finita." >>log/update.log
  echo "========================" >>log/update.log
fi

## Load some common functions:
. ${SDCARD}/scripts/common_functions.sh

mount -o bind ${SDCARD}/bin/busybox /bin/busybox
echo "Bind mounted ${SDCARD}/bin/busybox to /bin/busybox" >> $LOGPATH
mount -o bind /opt/root /root
echo "Bind mounted ${SDCARD}/root to /root" >> $LOGPATH
mount -o bind ${SDCARD}/etc /etc
echo "Bind mounted ${SDCARD}/etc to /etc" >> $LOGPATH

# init ir_led gpio :
init_gpio 46
# init white_led gpio :
init_gpio 81

## Set Hostname
hostname -F $CONFIGPATH/hostname.conf

## Set Timezone
set_timezone

## Autostart all enabled services:
echo $(date -Iseconds) " Komenco de servoj. " >>$LOGPATH
for nivelo in `ls --color=never config/autostart|awk '{print substr($0,1,2);}'|sort -un`
do
  for i in ${SDCARD}/config/autostart/${nivelo}*; do
    echo $(date -Iseconds) " Komenco de $i. " >>$LOGPATH
    $i >>$LOGPATH 2>&1 & 
  done
  wait
done

## Autostart startup userscripts
echo $(date -Iseconds) " Komenco de uzantoskriptoj. " >>$LOGPATH
for i in ${SDCARD}/config/userscripts/startup/*; do
  echo $(date -Iseconds) " Komenco de $i. " >>$LOGPATH
  $i &
done

