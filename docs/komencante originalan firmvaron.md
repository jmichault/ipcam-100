

la firmvaro konsistas el 6 sekcioj :
mtdblock0 = u-boot ŝargilo
mtdblock1 = starta agordo
mtdblock2 = "u-boot legacy uImage", linux-kerno
mtdblock3 = squashfs = /
mtdblock4 = squashfs = /ipc
mtdblock5 = jffs2 = /opt

La startprogramo estas "/linuxrc".
Ĝi lanĉas precipe /etc/init.d/rcS.
/etc/init.d/rcS lanĉas /ipc/etc/auto_run.sh
/ipc/etc/auto_run.sh lanĉas /opt/etc/local.rc antaŭ lanĉado de /ipc/app/jco_server.
/ipc/app/jco_server estas la ĉefa programo: ĝi faras kaj http-servilon, kaj rtsp-servilon, ktp...

