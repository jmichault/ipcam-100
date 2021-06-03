#!/bin/sh
. /etc/profile >/dev/null 2>&1


SDPATH=${DOCUMENT_ROOT}/..
. ${SDPATH}/www/cgi-bin/func.cgi
. ${SDPATH}/scripts/common_functions.sh

cat << EOF
  <form id="formPasv$F_logname" class='table-responsive'>
   <table class="table table-striped table-bordered" id="tabpasv$F_logname">
    <thead class="bg-info">
     <th scope="col" class="w-auto lang" data-lang="Nomo"> </th>
     <th scope="col" class="w-auto lang" data-lang="Pasvorto"> </th>
EOF

if [ x"$F_logname" == "x" -o x"$F_logname" == "x1" ]
then
 echo '    <th scope="col" class="w-auto lang" data-lang="Grupo<br>n-ro"> </th>'
 echo '    <th scope="col" class="w-auto lang" data-lang="Uzanto<br>n-ro"></th>'
 echo '    <th scope="col" class="w-auto lang" data-lang="Priskribo"></th>'
 echo '    <th scope="col" class="w-auto lang" data-lang="hejma dosierujo"></th>'
 echo '    <th scope="col" class="w-auto lang" data-lang="ŝelo"></th>'
 echo ' </thead> <tbody>'
 SRC=/etc/passwd
 echo "_:x">/tmp/pasvcr.$$
elif [ x"$F_logname" == "x2" ]
then
 echo ' </thead> <tbody>'
 SRC=$SDCARD/config/ipcam.user
 echo "_:***">/tmp/pasvcr.$$
elif [ x"$F_logname" == "x3" ]
then
 echo ' </thead> <tbody>'
 SRC=$SDCARD/config/rtsp.user
 echo "_:***">/tmp/pasvcr.$$
else
 echo ' </thead> <tbody>'
 echo "???"
fi

i=0
cat $SRC /tmp/pasvcr.$$| while IFS=":" read v1 v2 v3 v4 v5 v6 v7
do
 if [ "x$v1" == "x_" ]
 then
   v1=""
 fi
 if [ "x$v1" == "xroot" -o "x$v1" == "xwww-data" -o "x$v1" == "xrtsp" ]
 then
   echo "<tr><td> <input type='text' class='form-control w-100' name='nomo$i' value='$v1' readonly>"
 else
   echo "<tr><td> <input type='text' class='form-control w-100' name='nomo$i' value='$v1'>"
 fi
 echo "<input type='hidden' class='form-control w-100' name='nomo0_$i' value='$v1'></td>"
 echo "<td> <input type='password' class='form-control w-100' name='pasvorto$i' value='***'></td>"
 if [ x"$v2" == "xx" ]
 then
   if [ "x$v1" == "xroot" -o "x$v1" == "xwww-data" -o "x$v1" == "xrtsp" ]
   then
     echo "<td> <input type='number' min='0' max='9999' class='form-control' name='gid$i' value='$v3' readonly></td>"
     echo "<td> <input type='number' min='0' max='9999' class='form-control' name='uid$i' value='$v4' readonly></td>"
     echo "<td> <input type='text' class='form-control w-100' name='priskribo$i' value='$v5'></td>"
     echo "<td> <input type='text' class='form-control w-100' name='hejmo$i' value='$v6'></td>"
     echo "<td> <input type='text' class='form-control w-100' name='sxelo$i' value='$v7' readonly></td>"
   else
     echo "<td> <input type='number' min='0' max='9999' class='form-control' name='gid$i' value='$v3'></td>"
     echo "<td> <input type='number' min='0' max='9999' class='form-control' name='uid$i' value='$v4'></td>"
     echo "<td> <input type='text' class='form-control w-100' name='priskribo$i' value='$v5'></td>"
     echo "<td> <input type='text' class='form-control w-100' name='hejmo$i' value='$v6'></td>"
     echo "<td> <input type='text' class='form-control w-100' name='sxelo$i' value='$v7'></td>"
   fi
 fi
 echo "</tr>"
 i=$(($i+1))
done

echo ' </tbody> </table>'
echo '<button type="submit" class="btn btn-primary lang" data-lang="Submeti"></button>'
echo '</form>'

cat << EOF
<script>
\$(document).ready(function() {
  \$('#formPasv$F_logname').submit(function(event) {
    event.preventDefault();
    const data = new FormData(event.target);

    const value = Object.fromEntries(data.entries());
    var text=JSON.stringify(value);

    var XHR = new XMLHttpRequest();
    XHR.addEventListener("load", function(event) {
      \$('#output').html('<p> Changements enregistrés.</p>');
    });
    XHR.addEventListener("error", function(event) {
      \$('#output').html('<p> Erreur dans l enregistrement.</p>');
    });
    XHR.open("POST", "cgi-bin/pasv_trt.cgi?file=$SRC");
    XHR.send(  text );

  });
});
</script>

EOF

