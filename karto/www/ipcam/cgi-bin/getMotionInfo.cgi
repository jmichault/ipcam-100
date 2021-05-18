#!/bin/sh

echo "Content-type: application/json"
echo "Pragma: no-cache"
echo "Cache-Control: max-age=0, no-store, no-cache"
echo

. ${DOCUMENT_ROOT}/../scripts/common_functions.sh

motion_indicator_color=`${DOCUMENT_ROOT}/../bin/setconf -g z 2>/dev/null`
if [ "${motion_indicator_color}X" == "X" ]
then
    motion_indicator_color="0"
fi

motion_sensitivity=`${DOCUMENT_ROOT}/../bin/setconf -g m 2>/dev/null`
if [ "${motion_sensitivity}X" == "X" ]
then
    motion_sensitivity="0"
fi

region_of_interest=`${DOCUMENT_ROOT}/../bin/setconf -g r 2>/dev/null`
if [ "${region_of_interest}X" == "X" ]
then
    region_of_interest="0,0,0,0"
fi

motion_tracking=`${DOCUMENT_ROOT}/../bin/setconf -g t 2>/dev/null`
if [ "${motion_tracking}X" == "X" ]
then
    motion_tracking=false
fi

motion_timeout=`${DOCUMENT_ROOT}/../bin/setconf -g u 2>/dev/null`
if [ "${motion_timeout}X" == "X" ]
then
    motion_timeout=60
fi

process=`ps -l| grep v4l2rtspserver-master | grep -v grep`
w=`echo ${process}| awk -F '-W' '{print $2}' | awk '{print $1}'`
if [ "${w}X" == "X" ]
then
    w="1280"
fi

h=`echo ${process} | awk -F '-H' '{print $2}' | awk '{print $1}'`
if [ "${h}X" == "X" ]
then
    h="720"
fi

if [ "$(motion_detection status)" == "ON" ]
then
  motion_detection="true"
else
  motion_detection="false"
fi

echo "{\"motion_indicator_color\": ${motion_indicator_color},
\"motion_sensitivity\": ${motion_sensitivity},
\"motion_detection\": ${motion_detection},
\"region_of_interest\": [${region_of_interest}],
\"motion_tracking\": ${motion_tracking},
\"motion_timeout\": ${motion_timeout},
\"width\": ${w},
\"height\": ${h}}"
