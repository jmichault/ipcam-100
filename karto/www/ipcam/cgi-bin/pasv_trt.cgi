#!/bin/sh
. /etc/profile >/dev/null 2>&1

SDPATH=${DOCUMENT_ROOT}/..
. ${SDPATH}/www/cgi-bin/func.cgi

if [ x$F_file == x ]
then
  exit 1
fi

if [ x"$F_file" == "x/etc/passwd" ]
then
  rm /etc/passwd.$$
fi

data="$POST_QUERY_STRING"
for num in $(seq 0 100)
do
  nomo=$(echo "$data"|jq -r ".[\"nomo${num}\"]")
  nomo0=$(echo "$data"|jq -r ".[\"nomo0_${num}\"]")
  pasvorto=$(echo "$data"|jq -r ".[\"pasvorto${num}\"]")
  if [ x"$nomo0" == "xnull" ]
  then
    break;
  fi

 if [ x"$F_file" == "x/etc/passwd" ]
 then
  gid=$(echo "$data"|jq -r ".[\"gid${num}\"]")
  uid=$(echo "$data"|jq -r ".[\"uid${num}\"]")
  priskribo=$(echo "$data"|jq -r ".[\"priskribo${num}\"]")
  hejmo=$(echo "$data"|jq -r ".[\"hejmo${num}\"]")
  sxelo=$(echo "$data"|jq -r ".[\"sxelo${num}\"]")
  if [ x"$nomo" != "x" ]
  then
    echo "$nomo:x:$gid:$uid:$priskribo:$hejmo:$sxelo" >>/etc/passwd.$$
  fi
  if [ "x$nomo0" != "x" -a "x$nomo0" != "x$nomo" ]
  then
    if [ "x$nomo" == "x" ]
    then
      passwd -l "$nomo0"
    else
      cat /etc/shadow|sed "s/^$nomo0:/$nomo:/" >/tmp/shadow.$$
      mv /tmp/shadow.$$ /etc/shadow
    fi
  fi
  if [ x"$pasvorto" != "x***" -a x"$pasvorto" != "x" ]
  then
    if ! grep "^$nomo:" /etc/passwd
    then
      echo "$nomo:x:$gid:$uid:$priskribo:$hejmo:$sxelo" >>/etc/passwd
    fi
    if ! grep "^$nomo:" /etc/shadow
    then
      echo "$nomo:!:::::::" >>/etc/shadow
    fi
    echo "${pasvorto}
${pasvorto}"|$SDCARD/bin/busybox passwd "$nomo"  2>&1
  fi
 elif [ $(basename "$F_file") == "rtsp.user" ]
 then
  if [ "x$nomo0" != "x" -a "x$nomo0" != "x$nomo" ]
  then
    if [ "x$nomo" == "x" ]
    then
      grep -v "^$nomo0:" "$F_file" >/tmp/htpasstmp.$$
      mv /tmp/htpasstmp.$$ "$F_file"
    else
      cat "$F_file"|sed "s/^$nomo0:/$nomo:/" >/tmp/htpasstmp.$$
      mv /tmp/htpasstmp.$$ "$F_file"
    fi
  fi
  if [ x"$pasvorto" != "x***" -a x"$pasvorto" != "x" ]
  then
    grep -v "^$nomo0:" "$F_file" >/tmp/htpasstmp.$$
    echo "$nomo:$(echo -n "$nomo:RTSP server:$pasvorto"|md5sum|sed 's/ *- *//')" >>/tmp/htpasstmp.$$
    mv /tmp/htpasstmp.$$ "$F_file"
  fi
 else
  if [ "x$nomo0" != "x" -a "x$nomo0" != "x$nomo" ]
  then
    if [ "x$nomo" == "x" ]
    then
      grep -v "^$nomo0:" "$F_file" >/tmp/htpasstmp.$$
      mv /tmp/htpasstmp.$$ "$F_file"
    else
      cat "$F_file"|sed "s/^$nomo0:/$nomo:/" >/tmp/htpasstmp.$$
      mv /tmp/htpasstmp.$$ "$F_file"
    fi
  fi
  if [ x"$pasvorto" != "x***" -a x"$pasvorto" != "x" ]
  then
    htpasswd -b "$F_file" "$nomo" "${pasvorto}"
  fi
 fi

#echo $nomo="$valoro"
#export $nomo="$valoro"
done

if [ x"$F_file" == "x/etc/passwd" -a -s /etc/passwd.$$ ]
then
  mv /etc/passwd /etc/passwd-
  mv /etc/passwd.$$ /etc/passwd
fi

echo "Content-type: text/html"
echo "Pragma: no-cache"
echo "Cache-Control: max-age=0, no-store, no-cache"
echo ""
return 0

