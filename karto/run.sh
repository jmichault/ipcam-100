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

${SDCARD}/config/autostart/system-webserver

${SDCARD}/controlscripts/rtsp start

${SDCARD}/controlscripts/ftp-server start

