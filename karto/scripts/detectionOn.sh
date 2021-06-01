#!/bin/sh

# Source your custom motion configurations
. ${SDCARD}/scripts/common_functions.sh

motion_trigger_led=true
max_snapshots=20
save_dir=/opt/media/sdc/motion/stills
save_file_date_pattern="+%d-%m-%Y_%H.%M.%S"


.  JsAlVar ${SDCARD}/www/ipcam/config/movo.conf DetektoDatenoj


# Led
if [ "$motion_trigger_led" = true ] ; then
    led_blink 4 &
fi

# Save a snapshot
if [ x"$StokPicOn" == x1 ] ; then
	pattern="${save_file_date_pattern:-+%d-%m-%Y_%H.%M.%S}"
	filename=$(date $pattern).jpg
	if [ ! -d "$save_dir" ]; then
		mkdir -p "$save_dir"
	fi
	{
		# Limit the number of snapshots
		if [ "$(ls "$save_dir" | wc -l)" -ge "$max_snapshots" ]; then
			rm -f "$save_dir/$(ls -ltr "$save_dir" | awk 'NR==2{print $9}')"
		fi
	} &
	${SDCARD}/bin/getimage > "$save_dir/$filename" &
fi

# Send emails ...
if [ x"$SciiMailOn" == x1 ] ; then
    ${SDCARD}/scripts/sendPictureMail.sh&
fi

# Send a telegram message
if [ x"$SciiTelOn" == x1 ]; then
	if [ "$StokPicOn" == x1 ] ; then
		${SDCARD}/bin/telegram p "$save_dir/$filename"
	else
		${SDCARD}/bin/getimage > "/tmp/telegram_image.jpg"
 		${SDCARD}/bin/telegram p "/tmp/telegram_image.jpg"
 		rm "/tmp/telegram_image.jpg"
	fi
fi

