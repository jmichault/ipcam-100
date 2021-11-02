pliaj informoj ĉi tie : <https://jmichault.github.io/ipcam-100-dok/>

# celo:

krei alternativan firmvaron por ĉi tiu kamerao:

marko = WIFI CARAM  
modelo : WF-HD820-ZS  
rezolucio : 1080P  


DeviceName:     INGENIC-V01  
DeviceModel:    IPCAM-100  
DeviceSN:       12345678901  
KernelVersion:  Thu Jul 9 15:46:54 CST 2015  
ServerVersion:  WS3.53.1243-20190717-DANALE-GEN  
WebVersion:     V2.3.4-20190709  

procesoro : ingenic T21  
video-sentilo : GC2053

# Alternativa Firmvaro


## Kio funkcias en alternativa firmvaro:
* lan kaj wlan
* RTSP h264 kaj MJPEG servilo
* HTTP servilo
* motora kontrolo
* kalibrado de motoro
* PTZ agordoj
* NTP kliento
* blanka kaj IR LED kontrolo
* FTP servilo
* Video-Agordoj
* duaranga videorivereto
* movado-detekto
* mail
* telegram
* ŝanĝo de pasvorto por http-aliro,rtsp,ssh.
* aŭtomata nokta detekto
* Bildrigardilo

## Kio ne funkcias en alternativa firmvaro:
* sonosistemo
* OSD
* ĝisdatigo de firmvaro
* ir-cut
* mqtt
* ktp ...

## Instalado alternativan firmvaron :  
* Kopiu ĉion el la dosierujo "karto" en la radikon de la mikroSD.
* laŭvole agordu la reton (www/ipcam/config.dist/reto.conf)
* Enmetu la karton mikroSD.
* Konektu la kameraon al via loka reto kaj funkciigu la kameraon.
* Trovu la IP-adreson de la kamerao.
* (vi ankaŭ povas uzi la reton wifi de la kamerao: konekti vian komputilon al la reto _HSIPC-******_. Ĉi-kaze la IP-adreso de la kamerao estas 172.25.0.1) )
* Konektu al la kamerao per "telnet IP 9527" (uzanto root, pasvorto jco66688), ne pli ol 5 minutojn post la funkciigo de la kamerao.
* Faru "killall -9 auto_run.sh" por malebligi ke la konekto estu tranĉita.
* Faru "cd /opt/media/mmcblk0p1;./install.sh"
* Vi povas konekti al la URL "https://IP" (uzanto admin, pasvorto ismart21).
* Vi povas konekti per ssh (uzanto admin, pasvorto ismart12).
* ffplay -i rtsp://admin:ismart21@IP_adreso/stream1
* ffplay -i rtsp://admin:ismart21@IP_adreso/stream2
* 

## Malinstalado alternativan firmvaron :
* Konektu al la kamerao per ssh.
* Faru "cd /opt/media/mmcblk0p1;./uninstall.sh;reboot"
Vi ankaǔ povas simple forigi la karton mikroSD.

# Interesaj dokumentoj
Multa laboro estis farita por kameraoj ekipitaj per ingenic T20-procesoro.  
modifitaj firmvaroj :  https://github.com/EliasKotlyar/Xiaomi-Dafang-Hacks  
fontoj de modifitaj firmvaroj : https://github.com/Dafang-Hacks/Main  
https://github.com/openmiko/openmiko
lanĉante programojn sen ŝanĝi la firmvaro :  https://github.com/ThatUsernameAlreadyExist/JCO-PM203-Fisheye-Ingenic-T20-P2P-camera-hacks  
