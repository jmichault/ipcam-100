#!/bin/sh
. /etc/profile >/dev/null 2>&1

SDPATH=${DOCUMENT_ROOT}/..
. ${SDPATH}/www/cgi-bin/func.cgi

echo "Content-type: text/html"
echo "Pragma: no-cache"
echo "Cache-Control: max-age=0, no-store, no-cache"
echo ""

cat << EOF
Information:
<pre>Interfaces:<br/>$(ifconfig; iwconfig)</pre>
<pre>Routes:<br/>$(route)</pre>
<pre>DNS:<br/>$(cat /etc/resolv.conf)</pre>
</body>
</html>
EOF


