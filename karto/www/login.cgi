#!/bin/bash
. /etc/profile >/dev/null

. cgi-bin/func.cgi

../bin/htpasswd -b -v ../config/ipcam.user "$F_uname" "$F_psw" >/dev/null
if [ $? -ne 0 ]
then
echo '<html>
     <head>	
           <meta http-equiv="Refresh" content="0; url=/" />
     </head>'
echo "<body>"
echo "<h1> Bad Password </h1>"
echo "</body>"
echo "</html>"


else
SEC=`grep -w " *\"secret\"" ../config/lighttpd.conf|sed "s/.*=> *\"//;s/\".*//"`
../bin/tkt_token "$SEC"
echo '<html>
     <head>	
           <meta http-equiv="Refresh" content="0; url=/ipcam" />
     </head>'
echo "<body>"
echo "</body>"
echo "</html>"
fi
