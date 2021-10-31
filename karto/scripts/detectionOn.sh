#!/bin/sh

# Source your custom motion configurations
. ${SDCARD}/scripts/common_functions.sh

motion_trigger_led=true
# nombro da piktoj kaj videoj por konservi
max_pik=100
max_vid=20
# minimuma libera spaco sur la SDa karto 
min_lib=1000000
save_dir=${SDCARD}/motio
save_file_date_pattern="+%Y-%m-%d_%H.%M.%S"


#.  JsAlVar ${SDCARD}/www/ipcam/config/rtsp.conf config
#.  JsAlVar ${SDCARD}/www/ipcam/config/movo.conf DetektoDatenoj
. ${SDCARD}/config/varDetekto


# Led
if [ "$motion_trigger_led" = true ] ; then
    led_blink 4 &
fi

pattern="${save_file_date_pattern:-+%Y-%m-%d_%H.%M.%S}"
pat0=$(date $pattern)


# Komencu registri videon
if [ x"$StokMovOn" == x1 ] ; then
  if [ ! -d "$save_dir/vid" ]; then
    mkdir -p "$save_dir/vid"
  fi
  lokuz=$(cat ${SDCARD}/config/rtsp.lok|sed "s/:.*//")
  lokpas=$(cat ${SDCARD}/config/rtsp.lok|sed "s/.*://")
  largxo=$(echo $canal1taille|sed "s/x.*//")
  alteco=$(echo $canal1taille|sed "s/.*x//")
  video_duration=10
  video_temp=$pat0.mp4
  echo komencas la videoregistradon
  openRTSP -4 -w "$largxo" -h "$alteco" -f "$canal1fps" -d "$video_duration" rtsp://$lokuz:$lokpas@127.0.0.1/stream1 > "$save_dir/vid/$video_temp" &
fi


# Save a snapshot
if [ x"$StokPicOn" == x1 ] ; then
  echo komencas la ekrankopion
	filename=$pat0.jpg
	if [ ! -d "$save_dir/pik" ]; then
		mkdir -p "$save_dir/pik"
	fi
	${SDCARD}/bin/getimage > "$save_dir/pik/$filename" &
fi

# Send emails ...
if [ x"$SciiMailOn" == x1 ] ; then
  echo sendas retmesaĝon
    ${SDCARD}/scripts/sendPictureMail.sh &
fi

# Send a telegram message
if [ x"$SciiTelOn" == x1 ]; then
  echo sendas telegram-mesaĝon
	if [ "$StokPicOn" == x1 ] ; then
		${SDCARD}/bin/telegram p "$save_dir/pik/$filename"
	else
		${SDCARD}/bin/getimage > "/tmp/telegram_image$$.jpg"
 		${SDCARD}/bin/telegram p "/tmp/telegram_image$$.jpg"
 		rm "/tmp/telegram_image$$.jpg"
	fi
fi

wait

# limigi la nombron da videoj
echo forigas supernombrajn filmetojn
while [ "$(ls "$save_dir/vid" | wc -l)" -gt "$max_vid" ]; do
	rm -f "$save_dir/vid/$(ls -ltr "$save_dir/vid" | awk 'NR==2{print $9}')"
done
# limigi la nombron da bildoj
echo forigas supernombrajn bildojn
while [ "$(ls "$save_dir/pik" | wc -l)" -gt "$max_pik" ]; do
	rm -f "$save_dir/pik/$(ls -ltr "$save_dir/pik" | awk 'NR==2{print $9}')"
done
echo liberigas spacon 
while [ "$(df -k ${SDCARD}| awk 'NR==2{print $4}')" -lt "$min_lib" ]; do
  DOS=$(ls -lrt "$save_dir"/*/* | awk 'NR==1{print $9}')
  if [ "x$DOS" = "x" ]; then
    echo "neeble liberigi la deziratan spacon."
    break
  else
    rm "$DOS"
  fi
done
