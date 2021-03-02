

mtdblock0 = u-boot loader
mtdblock1 = config de boot
mtdblock2 = u-boot legacy uImage, noyau linux
mtdblock3 = squashfs = /
mtdblock4 = squashfs, monté sur /ipc
mtdblock5 = jffs2, monté sur /opt

u-boot charge le noyau en mtdblock2
options par défaut dans le u-boot loader:
bootargs=console=ttyS1,115200n8 mem=43M@0x0 rmem=21M@0x2B00000 init=/linuxrc rootfstype=squashfs root=/dev/mtdblock3
bootcmd=sf probe;sf read 0x80600000 0x48000 0x280000; bootm 0x80600000
bootdelay=1
baudrate=115200
loads_echo=1
ethaddr=00:11:22:33:44:74
ipaddr=192.168.2.84
serverip=192.168.2.81
gatewayip=192.168.2.1
netmask=255.255.255.0

options dans mtdblock1 :
baudrate=115200
bootcmd=sf probe;sf read 0x80600000 0x48000 0x280000; bootm 0x80600000
bootdelay=1
ethact=Jz4775-9161
gatewayip=192.168.2.1
ipaddr=192.168.2.84
loads_echo=1
netmask=255.255.255.0
serverip=192.168.2.81
stderr=serial
stdin=serial
stdout=serial
ethaddr=**:**:**:**:**:**
device_id=***********
devinfo=jcoxa***************************************************
bootargs=console=ttyS1,115200n8 mem=42M@0x0 rmem=22M@0x2A00000 init=/linuxrc rootfstype=squashfs root=/dev/mtdblock3 flash=SF sensor=GC2053 maxheight=1080 device_id=*********** ethaddr=**:**:**:**:**:** devinfo=jcoxa*************************************************** cpu=T21 ddr=64M mtdparts=jz_sfc:256K@0K(sf-bootloader),32K@256K(sf-bootenv),1440K@288K(sf-kernel),832K@1728K(sf-rootfs),4928K@2560K(sf-ipcfs),704K@7488K(sf-miscfs)


linux-versio :
Linux version 3.10.14__isvp_turkey_1.0__ (root@localhost.localdomain) (gcc version 4.7.2 (Ingenic r2.3.3 2016.12) ) #3 PREEMPT Sat Jun 22 10:40:55 CST 2019


dosieroj esencaj en / :
/bin/busybox : 
        [, [[, ash, awk, base64, basename, blockdev, bootchartd, bunzip2, bzcat, bzip2, cat, chmod, chown, cmp, cp, cut, date, dd, depmod, devmem, df,
        dhcprelay, diff, dirname, dmesg, dnsdomainname, du, dumpleases, echo, egrep, expr, fdflush, fdformat, fdisk, fgrep, find, flash_eraseall,
        flock, free, fuser, getty, grep, gzip, halt, hd, head, hexdump, hostname, hwclock, ifconfig, init, insmod, iostat, kill, killall, klogd, less,
        linuxrc, ln, logger, login, logread, ls, lsmod, md5sum, mdev, mesg, mkdir, mkdosfs, mkfs.vfat, mknod, mktemp, modinfo, modprobe, mount,
        mountpoint, mpstat, mv, nc, netstat, passwd, ping, pmap, poweroff, powertop, printf, ps, pstree, pwd, pwdx, readahead, reboot, rev, rm, rmdir,
        rmmod, route, sed, sh, sleep, smemcap, sort, stat, strings, swapoff, swapon, sync, sysctl, syslogd, tail, tar, tee, telnetd, test, tftp, time,
        timeout, top, touch, tr, true, tty, udhcpc, udhcpd, umount, unxz, unzip, uptime, users, usleep, vi, volname, watch, wc, which, xargs, xz,
        xzcat

/lib : normaj bibliotekoj

dosieroj esencaj en /ipc :
/ipc/app/jco_server
	ĉefa programo. 
/ipc/drv : linux-peliloj
	motor.ko
/ipc/lib : 
	libimp.so


note : libimp.so est différente de celle livrée pour le T20, et celle fournie avec le T20 ne convient pas.
