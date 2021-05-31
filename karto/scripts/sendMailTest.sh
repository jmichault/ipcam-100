#!/bin/sh
. /etc/profile 2>/dev/null

. /opt/media/sdc/scripts/update_timezone.sh

MAILDATE=$(date -R)

if [ ! -f ${SDCARD}/www/ipcam/config/posxto.conf ]
then
  echo "You must configure ${SDCARD}/www/ipcam/config/posxto.conf before using sendPictureMail or sendMailTest"
  exit 1
fi

. JsAlVar ${SDCARD}/www/ipcam/config/posxto.conf posxto_conf

if [ -f /tmp/sendPictureMail.lock ]; then
  rm /tmp/sendPictureMail.lock
fi

{

printf '%s\n' "From: ${FROM}
To: ${testoAl}
Subject: ${testoPri}
Date: ${MAILDATE}
Mime-Version: 1.0
Content-Type: text/plain; charset=\"US-ASCII\"
Content-Transfer-Encoding: 7bit
Content-Disposition: inline

${testoMesagxo}
"
} | busybox sendmail -v -H"exec /opt/media/sdc/bin/openssl s_client -quiet -connect $servilo:$haveno" -f"$adresanto" -au"$ensaluto" -ap"$pasvorto" $testoAl


