#!/bin/sh
. /etc/profile >/dev/null 2>&1

. ${SDCARD}/www/cgi-bin/func.cgi


SCRIPT_HOME="${SDCARD}/controlscripts/"
if [ -n "$F_script" ]; then
  script="${F_script##*/}"
  if [ -e "$SCRIPT_HOME/$script" ]; then
    case "$F_cmd" in
      start)
        echo "Content-type: text/html"
        echo ""

        echo "Running script '$script'..."
        echo "<pre>$("$SCRIPT_HOME/$script" 2>&1)</pre>"
        ;;  
      disable)
        rm "${SDCARD}/config/autostart/$script"
        komenco=`grep "^# Komenco:" "$SCRIPT_HOME/$script"|sed 's/^# Komenco: *//'`
        rm "${SDCARD}/config/autostart/${komenco}_$script"
        echo "Content-type: application/json"
        echo ""
        echo "{\"status\": \"ok\"}"
        ;;
      stop)
        echo "Content-type: text/html"
        echo ""
        status='unknown'
        echo "Stopping script '$script'..."
        echo "<pre>"
        "$SCRIPT_HOME/$script" stop 2>&1 && echo "OK" || echo "NOK"
        echo "</pre>"
        ;;
      restart)
        echo "Content-type: text/html"
        echo ""

        echo "Restarting script '$script'..."
        echo "<pre>$("$SCRIPT_HOME/$script" restart 2>&1)</pre>"
        ;;  
      enable)
        komenco=`grep "^# Komenco:" "$SCRIPT_HOME/$script"|sed 's/^# Komenco: *//'`
        echo "#!/bin/sh" > "${SDCARD}/config/autostart/${komenco}_$script"
        echo "$SCRIPT_HOME$script" >> "${SDCARD}/config/autostart/${komenco}_$script"
        echo "Content-type: application/json"
        echo ""
        echo "{\"status\": \"ok\"}"
        ;;
      view)
        echo "Content-type: text/html"
        echo ""
        echo "Contents of script '$script':"
        echo "<pre>$(cat "$SCRIPT_HOME/$script" 2>&1)</pre>"
        ;;
      *)
        echo "Content-type: text/html"
        echo ""
        echo "<p>Unsupported command '$F_cmd'</p>"
        ;;
    esac
  else
    echo "Content-type: text/html"
    echo ""
    echo "<p>$F_script is not a valid script!</p>"
  fi
  return
fi

cat << EOF
<div class='card status_card'>
 <header class='card-header bg-primary text-white'><p class='card-header-title lang' data-lang="Serviloj"></p></header>
 <div class='card-body'>
  <div class='content'>
   <table class="table table-striped table-bordered">
    <thead>
     <th scope="col" class="lang" data-lang="Nomo"> </th>
     <th scope="col" class="lang" data-lang="Aŭtomate komenci"> </th>
     <th scope="col" class="lang" data-lang="Kontrolo"> </th>
     <th scope="col" class="lang" data-lang="Priskribo"> </th>
     <th scope="col" class="lang" data-lang="Vidu la fonton"> </th>
    </thead>
    <tbody>
EOF


if [ ! -d "$SCRIPT_HOME" ]; then
  echo "<p>No scripts found in $SCRIPT_HOME</p>"
else
  SCRIPTS=$(/bin/ls -A --color=never "$SCRIPT_HOME")

  for i in $SCRIPTS; do
    retadmin=`grep "^# Retejo-administrita:" "$SCRIPT_HOME/$i"|sed 's/^# Retejo-administrita: *//'`
    if [ x$retadmin != xies ]
    then
      continue
    fi
    # Nomo
    if grep -q "^status()" "$SCRIPT_HOME/$i"; then
      status=$("$SCRIPT_HOME/$i" status)
      if [ $? -eq 0 ]; then
        if [ -n "$status" ]; then
          badge="";
        else 
          badge="badge-warning";
        fi
      else
        badge="badge-danger"
        status="NOK"
      fi
      echo "<tr><td class='align-middle'><div class='badge $badge' data-badge='$status'>$i</div></td>"
    else
        echo "<tr><td class='align-middle'>$i</td>"
    fi
    # Auxtomata komenco
    echo "<td class='align-middle'>"
    echo "<input type='checkbox' id='autorun_$i' name='autorun_$i' class='switch is-rtl autostart' data-script='$i' "
    echo " data-unchecked='cgi-bin/scripts.cgi?cmd=disable&script=$i'"
    echo " data-checked='cgi-bin/scripts.cgi?cmd=enable&script=$i'"
    komenco=`grep "^# Komenco:" "$SCRIPT_HOME/$i"|sed 's/^# Komenco: *//'`
    if [ -f $SCRIPT_HOME/../config/autostart/${komenco}_$i ]
    then
        echo " checked='checked'"
    fi
    echo "'>"
    echo "<label for='autorun_$i'>Autorun</label>"
    echo "</td>"

    # Start / Stop / Run buttons
    echo "<td><div class='buttons'>"
    if grep -q "^start()" "$SCRIPT_HOME/$i"; then
      echo "<button data-target='cgi-bin/scripts.cgi?cmd=start&script=$i' class='btn btn-secondary is-link script_action_start' data-script='$i' "
      if [ ! -z "$status" ]; then
        echo "disabled"
      fi
      echo ">Start</button>"
    else
      echo "<button data-target='cgi-bin/scripts.cgi?cmd=start&script=$i' class='btn btn-secondary is-link script_action_start' data-script='$i' "
      echo ">Run</button>"
    fi

    if grep -q "^stop()" "$SCRIPT_HOME/$i"; then
      echo "<button data-target='cgi-bin/scripts.cgi?cmd=stop&script=$i' class='btn btn-secondary is-danger script_action_stop' data-script='$i' "
      if [ ! -n "$status" ]; then
        echo "disabled"
      fi
      echo ">Stop</button>"
    fi
    if grep -q "^restart()" "$SCRIPT_HOME/$i"; then
      echo "<button data-target='cgi-bin/scripts.cgi?cmd=restart&script=$i' class='btn btn-secondary is-danger script_action_restart' data-script='$i' "
      if [ ! -n "$status" ]; then
        echo "disabled"
      fi
      echo ">Restart</button>"
    fi
    echo "</td>"
    # Priskribo
    priskribo=`grep "^# Mallonga-priskribo:" "$SCRIPT_HOME/$i"|sed 's/^# Mallonga-priskribo: *//'`
    echo "<td>$priskribo</td>"
    # View link
    echo "<td><a href='cgi-bin/scripts.cgi?cmd=view&script=$i' class='align-middle card-footer-item view_script' data-script="$i">Vidi</a></td>"
    echo "</tr>"
  done
fi
echo "</tbody></table></div></div></div>"

echo "<script src="js/scripts.cgi.js"></script>"
