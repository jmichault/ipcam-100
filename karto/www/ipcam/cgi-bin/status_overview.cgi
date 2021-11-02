#!/bin/sh

source ${DOCUMENT_ROOT}/../scripts/common_functions.sh

echo "Content-type: text/html"
echo "Pragma: no-cache"
echo "Cache-Control: max-age=0, no-store, no-cache"
echo ""

FS=`df ${DOCUMENT_ROOT}/..|tail -n 1|awk '{print $6;}'`
mount|grep -w ${FS}|grep "rw,">/dev/null

if [ $? == 1 ]; then

cat << EOF
  <!-- sdcard warning -->
  <article class="message is-warning">
    <div class="message-header">
      <p class="lang" data-lang="Averto"></p>
      <button class="delete" aria-label="delete"></button>
    </div>
    <div class="message-body">
      <a lang" data-lang="Via sdcard estas muntita nurlegebla. Agordoj ne konserveblas."></a>
      <br>
      <p class="lang" data-lang="Bonvolu provi restartigi."></p>
    </div>
  </article>
  <!-- end sdcard warning -->
EOF

fi

cat << EOF
<!-- Sistemo -->
<div class='card status_card'>
    <header class='card-header bg-primary text-white'><p class='card-header-title lang' data-lang="Sistemo"></p></header>
    <div class='card-body'>
        <div class='content'>
            <table class="table table-striped table-bordered">
              <tbody>
                <tr>
                  <td class="lang" data-lang="Gastnomo"> </td>
                  <td> $(hostname) </td>
                </tr>
                <tr>
                  <td class="lang" data-lang="Modelo"></td>
                  <td> $(detect_model) </td>
                </tr>
                <tr>
                  <td class="lang" data-lang="Firmprogramo"></td>
                  <td> $(if [ -s "${DOCUMENT_ROOT}/../.lastCommitDate" ]; then cat ${DOCUMENT_ROOT}/../.lastCommitDate; else echo "Update for version string"; fi) </td>
                </tr>
                <tr>
                  <td class="lang" data-lang="Kerno Versio"></td>
                  <td> $(${DOCUMENT_ROOT}/../bin/busybox uname -v) </td>
                </tr>
                <tr>
                  <td class="lang" data-lang="Praŝargilo Versio"></td>
                  <td> $(busybox strings /dev/mtd0 | grep "U-Boot 2") </td>
                <tr>
                <tr>
                  <td class="lang" data-lang="Loka Tempo"></td>
                  <td> $(date) </td>
                </tr>
                <tr>
                  <td class="lang" data-lang="Daŭro de funkciado"></td>
                  <td> $(uptime | sed 's/^.*up *//;s/, *[0-9]* user.*$/m/; s/ day[^0-9]*/d, /;s/ \([hms]\).*m$/\1/;s/:/h, /') </td>
                </tr>
                <tr>
                  <td class="lang" data-lang="Ŝarĝa Mezumo"></td>
                  <td> $(uptime | awk -F': ' '{print $2}') </td>
                </tr>
              </tbody>
            </table>
        </div>
    </div>
</div>

<!-- Reto -->
<div class='card status_card'>
 <header class='card-header bg-primary text-white'><p class='card-header-title lang' data-lang="Reto (WLAN0)"></p></header>
 <div class='card-body'>
  <div class='content'>
   <table class="table table-striped table-bordered">
    <tbody>
     <tr>
      <td>SSID </td>
      <td> $(/system/bin/iwgetid -r) </td>
     </tr>
     <tr>
      <td class="lang" data-lang="Liga Kvalito"></td>
      <td> $(cat /proc/net/wireless | awk 'END { print $3 }' | sed 's/\.$//') </td>
     </tr>
     <tr>
      <td class="lang" data-lang="IP-Adreso"></td>
      <td> $(ifconfig | grep -E "([0-9]{1,3}\.){3}[0-9]{1,3}" | grep -v 127.0.0.1 | awk '{ print $2 }' | cut -f2 -d:) </td>
     </tr>
     <tr>
      <td class="lang" data-lang="MAC-Adreso"></td>
      <td> $(cat /sys/class/net/wlan0/address) </td>
     </tr>
     <tr>
      <td class="lang" data-lang="Retmasko"></td>
      <td> $(ifconfig wlan0 | sed -rn '2s/ .*:(.*)$/\1/p') </td>
     </tr>
     <tr>
      <td class="lang" data-lang="Kluzadreso"></td>
      <td> $(route | awk '/default/ { print $2}') </td>
     </tr>
     <tr>
      <td>DNS</td>
      <td> <pre>$(cat /etc/resolv.conf) </pre></td>
     </tr>
    </tbody>
   </table>
  </div>
 </div>
</div>

<!-- Bootloader -->
<div class='card status_card'>
 <header class='card-header bg-primary text-white'><p class='card-header-title lang' data-lang="Praŝarĝilo"></p></header>
 <div class='card-body'>
  <div class='content'>
   <table class="table table-striped table-bordered">
    <tbody>
     <tr>
      <td class='lang' data-lang="MD5-Sumo de Praŝarĝilo :"></td>
      <td>$(md5sum /dev/mtd0 |cut -f 1 -d " ")</td>
     </tr>
     <tr>
      <td class="lang" data-lang="Versio de Praŝarĝilo :"></td>
      <td>$(busybox strings /dev/mtd0 | grep "U-Boot 2")</td>
     </tr>
     <tr>
      <td class="lang" data-lang="Komandlinio :"></td>
      <td>$(cat /proc/cmdline)</td>
     </tr>
    </tbody>
   </table>
  </div>
  <a target="_blank" href="cgi-bin/dumpbootloader.cgi" class="lang" data-lang="Elŝutu Praŝarĝilo"></a>
 </div>
</div>



EOF
