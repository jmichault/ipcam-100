celo: krei alternativan firmvaron por ĉi tiu kamerao:

marko = WIFI CARAM  
modelo : WF-HD820-ZS  
rezolucio : 1080P  


DeviceName:     INGENIC-V01  
DeviceModel:    IPCAM-100  
DeviceSN:       30901607543  
KernelVersion:  Thu Jul 9 15:46:54 CST 2015  
ServerVersion:  WS3.53.1243-20190717-DANALE-GEN  
WebVersion:     V2.3.4-20190709  

procesoro : ingenic T21  
video-sentilo : GC2053

Multa laboro estis farita por kameraoj ekipitaj per inĝenia T20-procesoro.  
modifitaj firmvaroj :  https://github.com/EliasKotlyar/Xiaomi-Dafang-Hacks  
fontoj de modifitaj firmvaroj : https://github.com/Dafang-Hacks/Main  
lanĉante programojn sen ŝanĝi la firmvaro :  https://github.com/ThatUsernameAlreadyExist/JCO-PM203-Fisheye-Ingenic-T20-P2P-camera-hacks  


malfermaj havenoj :  
  tcp : 80(http), 443(http ankaŭ !), 554(rtsp), 8004, 8006, 9527(telnetd), 9999  
  udp : 67(dhcp), 3702, 8002, 39642  

Vi povas konekti per telnet al la haveno 9527 dum kvin minutoj post la startigo. Uzanto: root, pasvorto: jco66688.  
(jco = http://www.cnjabsco.com/ )

Kio funkcias en alternativa firmvaro:
* lan kaj wlan
* RTSP MJPEG servilo
* HTTP servilo
* motora kontrolo
* kalibrado de motoro
* NTP kliento
* ŝanĝo de pasvorto por http-aliro
* blua kaj IR LED kontrolo


Kio ne funkcias en alternativa firmvaro:
* video sistemo pravalorizado  (ĝi devas esti pravalorizita per la originala firmvaro)
* Video-Agordoj
* sondosistemo
* OSD
* ĝisdatigo de firmvaro
* aŭtomata nokta detekto
* movado-detekto
* ir-cut
* mail
* mqtt
* h264
* ktp ...

Preparado de la kameraon :  
* Konektu la kameraon al via loka reto kaj funkciigu la kameraon.
* Trovu la IP-adreson de la fotilo.
* Konektu al la kamerao per la URL "http://IP" (uzanto admin, pasvorto admin)
* en la langeto "Audio & Video" starigu la ĉefan rivereton al 720p, alklaku "Save".
* laŭvole agordu la kablan reton en la langeto "Network".

Instalado alternativan firmvaron :  
* Kopiu ĉion el la dosierujo "karto" en la radikon de la microSD.
* laŭvole agordu la reton (config/staticip.conf, config/resolv.conf, config/wpa_supplicant.conf)
* Enmetu la karton microSD.
* Konektu la kameraon al via loka reto kaj funkciigu la kameraon.
* Konektu al la kamerao per "telnet IP 9527" (uzanto root, pasvorto jco66688).
* Faru "killall -9 auto_run.sh" por eviti fermi la konekton.
* Faru "cd /opt/media/mmcblk0p1;./install.sh"
* Vi povas konekti al la URL "https://IP" (uzanto admin, pasvorto jco66688).

Malinstalado alternativan firmvaron :
* Konektu al la kamerao per "telnet IP 9527" (uzanto root, pasvorto ismart12).
* Faru "killall -9 run.sh system-emergency-telnet" por eviti fermi la konekton.
* Faru "cd /opt/media/mmcblk0p1;./uninstall.sh;reboot"

