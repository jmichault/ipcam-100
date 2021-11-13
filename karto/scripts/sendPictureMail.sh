#!/bin/sh
. /etc/profile >/dev/null 2>&1

. ${SDCARD}/scripts/update_timezone.sh

. JsAlVar ${SDCARD}/www/ipcam/config/movo.conf DetektoDatenoj

boundary="ZZ_/afg6432dfgkl.94531q"
FILENAME=$(date "+%Y%m%d%H%M%S-")
NUMBEROFPICTURES=5
TIMEBETWEENSNAPSHOT=1
SUBJECT="Alerto de kamerao"
BODY="Alerto de `hostname`"



MAILDATE=$(date -R)

if [ ! -f ${SDCARD}/www/ipcam/config/posxto.conf ]
then
  echo "You must configure ${SDCARD}/www/ipcam/config/posxto.conf before using sendPictureMail"
  exit 1
fi

. JsAlVar ${SDCARD}/www/ipcam/config/posxto.conf posxto_conf



if [ -f /tmp/sendPictureMail.lock ]; then
  echo "sendPictureEmail already running, /tmp/sendPictureMail.lock is present"
  exit 1
fi

touch /tmp/sendPictureMail.lock

# Build headers of the emails
{

printf '%s\n' "From: ${adresanto}
To: ${SciiMailAl}
Subject: ${SUBJECT}
Date: ${MAILDATE}
Mime-Version: 1.0
Content-Type: multipart/mixed; boundary=\"$boundary\"

--${boundary}
Content-Type: text/plain; charset=\"US-ASCII\"
Content-Transfer-Encoding: 7bit
Content-Disposition: inline

${BODY}
"
for i in $(busybox seq 1 ${NUMBEROFPICTURES})
do
    # now loop over
    # and produce the corresponding part,
    printf '%s\n' "--${boundary}
Content-Type: image/jpeg
Content-Transfer-Encoding: base64
Content-Disposition: attachment; filename=\"${FILENAME}${i}.jpg\"
"

        ${SDCARD}/bin/getimage | ${SDCARD}/bin/openssl enc -base64

    echo

    if [ ${i} -lt ${NUMBEROFPICTURES} ]
    then
        sleep ${TIMEBETWEENSNAPSHOT}
    fi
done

# print last boundary with closing --
printf '%s\n' "--${boundary}--"
printf '%s\n' "-- End --"

} | busybox sendmail -H"exec ${SDCARD}/bin/openssl s_client -quiet -connect $servilo:$haveno" -f"$adresanto" -au"$ensaluto" -ap"$pasvorto" $SciiMailAl

rm /tmp/sendPictureMail.lock
