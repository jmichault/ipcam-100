#!/bin/sh
. /etc/profile >/dev/null 2>&1

TIME_ZONE_CONFIG_FILE="${SDCARD}/config/timezone.conf"

if [ ! -f $TIME_ZONE_CONFIG_FILE ]; then
    cp $TIME_ZONE_CONFIG_FILE.dist $TIME_ZONE_CONFIG_FILE
fi

time_zone=$(cat $TIME_ZONE_CONFIG_FILE)
export TZ="$time_zone"
