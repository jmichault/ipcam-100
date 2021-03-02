kun originala firmvaro :
* Konektu al la kamerao per "telnet IP 9527" (uzanto root, pasvorto jco66688).
* Faru "killall -9 auto_run.sh" por eviti fermi la konekton.

kun alternativa firmvaro :
* Konektu al la kamerao per "telnet IP 9527" (uzanto root, pasvorto ismart12).
* Faru "killall -9 run.sh system-emergency-telnet" por eviti fermi la konekton.
* /opt/media/mmcblk0p1/www/../controlscripts/rtsp-mjpeg stop


 * komenci «gdbserver» ekde la kamerao, ekz. :
	/opt/media/mmcblk0p1/bin/gdbserver localhost:2000 /ipc/app/jco_server	
 * komenci gdb en la komputilo , ekz. :
	cd programada_medio/toolchain
	bin/mips-linux-uclibc-gnu-gdb ...../mtdblock4/app/jco_server
	target remote 192.168.xxx.xxx:2000
	handle SIGPIPE nostop
	handle SIG32 nostop
	break xxx
	continue
