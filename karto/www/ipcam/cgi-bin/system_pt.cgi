#!/bin/sh

echo "Content-type: text/html"
echo "Pragma: no-cache"
echo "Cache-Control: max-age=0, no-store, no-cache"
echo ""

# source header.cgi

FS=`df ${DOCUMENT_ROOT}/..|tail -n 1|awk '{print $6;}'`
mount|grep -w ${FS}|grep "rw,">/dev/null

if [ $? == 1 ]; then

cat << EOF
  <!-- sdcard warning -->
  <article class="message is-warning">
    <div class="message-header">
      <p>Warning</p>
      <button class="delete" aria-label="delete"></button>
    </div>
    <div class="message-body">
      Your sdcard is mounted read-only. Settings can't be saved.
      <br>
      <p>Please try rebooting. If the problem persists, please <a target="_blank" href="https://github.com/EliasKotlyar/Xiaomi-Dafang-Hacks/search?q=read+only+sdcard&type=Issues">search
      for possible solutions.</a></p>
    </div>
  </article>
  <!-- end sdcard warning -->
EOF

fi

cat << EOF
<script>
    function call(url){
            var xhr = new XMLHttpRequest();
            xhr.open('GET', url, true);
            xhr.send();
    }

</script>

<!-- Motoro -->
<div class='card status_card'>
    <header class='card-header'><p class='card-header-title'>Motoro</p></header>
    <div class='card-content'>
        <table class="motor_control">
            <tr>
                <td></td>
                <td>
                    <button class="button is-link" onclick="call('cgi-bin/action.cgi?cmd=motor_up&val='+document.getElementById('val').value)">&uarr; Supra</button>
                </td>
                <td></td>
            </tr>
            <tr>
                <td>
                    <button class="button is-link" onclick="call('cgi-bin/action.cgi?cmd=motor_left&val='+document.getElementById('val').value)">&larr; Maldekstra</button>
                </td>
                <td>
                    <input class="input has-text-centered" type="text" id="val" name="val" value="100">
                </td>
                <td>
                    <button class="button is-link" onclick="call('cgi-bin/action.cgi?cmd=motor_right&val='+document.getElementById('val').value)">Dekstra &rarr;</button>
                </td>
            </tr>
            <tr>
                <td></td>
                <td>
                    <button class="button is-link" onclick="call('cgi-bin/action.cgi?cmd=motor_down&val='+document.getElementById('val').value)">&darr; Malsupra</button>
                </td>
                <td></td>
            </tr>
        </table>
	<br>
        <div class="buttons">
         <button class="button is-warning" onclick="call('cgi-bin/action.cgi?cmd=motor_calibrate')">Kalibri</button>
         <button class="button is-link" onclick="call('cgi-bin/action.cgi?cmd=motoro_poz&val=0')">P0</button>
         <button class="button is-link" onclick="call('cgi-bin/action.cgi?cmd=motoro_poz&val=1')">P1</button>
         <button class="button is-link" onclick="call('cgi-bin/action.cgi?cmd=motoro_poz&val=2')">P2</button>
         <button class="button is-link" onclick="call('cgi-bin/action.cgi?cmd=motoro_poz&val=3')">P3</button>
         <button class="button is-link" onclick="call('cgi-bin/action.cgi?cmd=motoro_poz&val=4')">P4</button>
        </div>
    </div>
</div>

<div class='card status_card'>
 <header class='card-header'><p class='card-header-title'>Agordoj</p></header>
 <div class='card-content'>
  <form id="formMotoro" action="cgi-bin/action.cgi?cmd=set_motoro" method="post">
   <div class='field is-horizontal'>
    <label class="label">Rapideco&nbsp:&nbsp</label>
    <input class="input" id="rapideco" name="rapideco" type="text" size="5" value="$(source ${DOCUMENT_ROOT}/../config/ptz.conf; echo $RAPIDECO)" />
   </div>
   <br>
   <p> X maksimumo : 1260 ; Y maksimumo : 3180</p>

   <div class='field is-horizontal'>
        <label class="label">Pozicio&nbsp0&nbsp:&nbspX=&nbsp</label><br>
         <input class="input" id="x0" name="x0" type="text" size="5" value="$(source ${DOCUMENT_ROOT}/../config/ptz.conf; echo $X0)" />
        <label class="label">&nbspY=&nbsp</label>
         <input class="input" id="y0" name="y0" type="text" size="5" value="$(source ${DOCUMENT_ROOT}/../config/ptz.conf; echo $Y0)" />
   </div>

   <div class='field is-horizontal'>
        <label class="label">Pozicio&nbsp1&nbsp:&nbspX=&nbsp</label><br>
         <input class="input" id="x1" name="x1" type="text" size="5" value="$(source ${DOCUMENT_ROOT}/../config/ptz.conf; echo $X1)" />
        <label class="label">&nbspY=&nbsp</label>
         <input class="input" id="y1" name="y1" type="text" size="5" value="$(source ${DOCUMENT_ROOT}/../config/ptz.conf; echo $Y1)" />
   </div>

   <div class='field is-horizontal'>
        <label class="label">Pozicio&nbsp2&nbsp:&nbspX=&nbsp</label><br>
         <input class="input" id="x2" name="x2" type="text" size="5" value="$(source ${DOCUMENT_ROOT}/../config/ptz.conf; echo $X2)" />
        <label class="label">&nbspY=&nbsp</label>
         <input class="input" id="y2" name="y2" type="text" size="5" value="$(source ${DOCUMENT_ROOT}/../config/ptz.conf; echo $Y2)" />
   </div>

   <div class='field is-horizontal'>
        <label class="label">Pozicio&nbsp3&nbsp:&nbspX=&nbsp</label><br>
         <input class="input" id="x3" name="x3" type="text" size="5" value="$(source ${DOCUMENT_ROOT}/../config/ptz.conf; echo $X3)" />
        <label class="label">&nbspY=&nbsp</label>
         <input class="input" id="y3" name="y3" type="text" size="5" value="$(source ${DOCUMENT_ROOT}/../config/ptz.conf; echo $Y3)" />
   </div>

   <div class='field is-horizontal'>
        <label class="label">Pozicio&nbsp4&nbsp:&nbspX=&nbsp</label><br>
         <input class="input" id="x4" name="x4" type="text" size="5" value="$(source ${DOCUMENT_ROOT}/../config/ptz.conf; echo $X4)" />
        <label class="label">&nbspY=&nbsp</label>
         <input class="input" id="y4" name="y4" type="text" size="5" value="$(source ${DOCUMENT_ROOT}/../config/ptz.conf; echo $Y4)" />
   </div>

   <div class="field is-horizontal">
    <div class="field-body">
     <div class="field">
      <div class="control">
       <input id="resSubmit" class="button is-primary" type="submit" value="Set" />
      </div>
     </div>
    </div>
   </div>
  </form>
 </div>
</div>

EOF
script=$(cat ${DOCUMENT_ROOT}/../www/js/status.cgi.js)
echo "<script>$script</script>"
