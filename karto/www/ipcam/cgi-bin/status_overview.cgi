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
      <p>Please try rebooting. If the problem persists, please <a target="_blank" href="https://github.com/EliasKotlyar/Xiaomi-Dafang-Hacks/search?q=read+only+sdcard&type=Issues">search
      for possible solutions.</a></p>
    </div>
  </article>
  <!-- end sdcard warning -->
EOF

fi

cat << EOF
<!-- Sistemo -->
<div class='card status_card'>
    <header class='card-header'><p class='card-header-title lang' data-lang="Sistemo"></p></header>
    <div class='card-content'>
        <div class='content'>
            <table>
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

<!-- Network -->
<div class='card status_card'>
    <header class='card-header'><p class='card-header-title'>Network (WLAN0)</p></header>
    <div class='card-content'>
        <div class='content'>
            <table>
              <tbody>
                <tr>
                  <td> SSID </td>
                  <td> $(/system/bin/iwgetid -r) </td>
                </tr>
                <tr>
                  <td> Link Quality </td>
                  <td> $(cat /proc/net/wireless | awk 'END { print $3 }' | sed 's/\.$//') </td>
                </tr>
                <tr>
                  <td> IP Address </td>
                  <td> $(ifconfig | grep -E "([0-9]{1,3}\.){3}[0-9]{1,3}" | grep -v 127.0.0.1 | awk '{ print $2 }' | cut -f2 -d:) </td>
                </tr>
                <tr>
                  <td> MAC Address </td>
                  <td> $(cat /sys/class/net/wlan0/address) </td>
                </tr>
                <tr>
                  <td> Netmask </td>
                  <td> $(ifconfig wlan0 | sed -rn '2s/ .*:(.*)$/\1/p') </td>
                </tr>
                <tr>
                  <td> Gateway </td>
                  <td> $(route | awk '/default/ { print $2}') </td>
                </tr>
                <tr>
                  <td> DNS </td>
                  <td> <pre>$(cat /etc/resolv.conf) </pre></td>
                </tr>
              </tbody>
            </table>
        </div>
    </div>
</div>

<!-- Bootloader -->
<div class='card status_card'>
    <header class='card-header'><p class='card-header-title'>Bootloader</p></header>
    <div class='card-content'>
        Bootloader MD5:
        <pre>$(md5sum /dev/mtd0 |cut -f 1 -d " ")</pre>
        Bootloader Version:
        <pre>$(busybox strings /dev/mtd0 | grep "U-Boot 2")</pre>
        Your CMDline is:
        <pre>$(cat /proc/cmdline)</pre>


        <a target="_blank" href="cgi-bin/dumpbootloader.cgi">Download Bootloader</a>
    </div>
</div>



EOF
