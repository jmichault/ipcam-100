#!/bin/sh
. /etc/profile >/dev/null 2>&1

SDPATH=${DOCUMENT_ROOT}/..
. ${SDPATH}/www/cgi-bin/func.cgi

if [x$F_file == x ]
then
  exit 1
fi

#FICOUT=${SDCARD}/www/config/rtsp.conf
FICOUT=/opt/media/mmcblk0p1/www/ipcam/config/$F_file

echo "$POST_QUERY_STRING" >$FICOUT

echo "Content-type: text/html"
echo "Pragma: no-cache"
echo "Cache-Control: max-age=0, no-store, no-cache"
echo ""
return 0

