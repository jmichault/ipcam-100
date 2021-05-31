#!/bin/sh
. /etc/profile >/dev/null 2>&1

SDPATH=${DOCUMENT_ROOT}/..
. ${SDPATH}/www/cgi-bin/func.cgi

echo "Content-type: text/html"
echo "Pragma: no-cache"
echo "Cache-Control: max-age=0, no-store, no-cache"
echo ""

cat << EOF

<div class='card status_card'>
  <header class='card-header bg-primary text-white'><p class='card-header-title lang' data-lang="Reto Informoj"></p></header>
  <div class='card-body'>
<pre>Interfaces:<br/>$(ifconfig; iwconfig)</pre>
<pre>Routes:<br/>$(route)</pre>
<pre>DNS:<br/>$(cat /etc/resolv.conf)</pre>
  </div>
</div>
EOF


