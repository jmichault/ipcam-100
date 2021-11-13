#!/bin/sh

# Source your custom motion configurations
. ${SDCARD}/scripts/common_functions.sh

motion_trigger_led=true
# nombro da piktoj kaj videoj por konservi
max_pik=100
max_vid=20
# minimuma libera spaco sur la SDa karto 
min_lib=1000000
save_subdir=$(date +%Y-%m-%d)
pat0=$(date +%H.%M.%S)

#.  JsAlVar ${SDCARD}/www/ipcam/config/rtsp.conf config
#.  JsAlVar ${SDCARD}/www/ipcam/config/movo.conf DetektoDatenoj
. ${SDCARD}/config/varDetekto


# Led
if [ "$motion_trigger_led" = true ] ; then
    led_blink 4 &
fi


# Komencu registri videon
if [ x"$StokMovOn" == x1 ] ; then
  save_dir=$SDCARD/motio/vid/$save_subdir/
  if [ ! -d "$save_dir" ]; then
    mkdir -p "$save_dir"
  fi
  lokuz=$(cat ${SDCARD}/config/rtsp.lok|sed "s/:.*//")
  lokpas=$(cat ${SDCARD}/config/rtsp.lok|sed "s/.*://")
  largxo=$(echo $canal1taille|sed "s/x.*//")
  alteco=$(echo $canal1taille|sed "s/.*x//")
  video_duration=10
  video_temp=$pat0.mkv
  echo komencas la videoregistradon
  #openRTSP -4 -w "$largxo" -h "$alteco" -f "$canal1fps" -d "$video_duration" rtsp://$lokuz:$lokpas@127.0.0.1/stream1 > "$save_dir/$video_temp" &
  ffmpeg-min-recorder -i rtsp://$lokuz:$lokpas@127.0.0.1/stream1 -vcodec copy -vframes 300 "$save_dir/$video_temp" &
fi


# Save a snapshot
if [ x"$StokPicOn" == x1 ] ; then
  echo komencas la ekrankopion
  save_dir=$SDCARD/motio/pik/$save_subdir/
  if [ ! -d "$save_dir" ]; then
    mkdir -p "$save_dir"
  fi
  filename=$pat0.jpg
  ${SDCARD}/bin/getimage > "$save_dir/$filename" &
fi

# Send emails ...
if [ x"$SciiMailOn" == x1 ] ; then
  echo sendas retmesaĝon
    ${SDCARD}/scripts/sendPictureMail.sh &
fi

wait

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
