
* Konektu al la kamerao per "telnet IP 9527" (uzanto root, pasvorto jco66688).
* Faru "killall -9 auto_run.sh" por eviti fermi la konekton.

* Konektu al la kamerao per "telnet IP 9527" (uzanto root, pasvorto ismart12).
* Faru "killall -9 run.sh system-emergency-telnet" por eviti fermi la konekton.


 * komenci «gdbserver» ekde la kamerao, ekz. :
	gdbserver localhost:2000 jco_server	
 * komenci gdb en la komputilo , ekz. :
	gdb jco_server
	target remote 192.168.24.111:2000
	break xxx
	continue
