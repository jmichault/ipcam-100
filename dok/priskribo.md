

# Priskribo :
https://fr.aliexpress.com/item/4000146161270.html
    Brand Name: shiningpo
    Style: Dome Camera
    Type: IP Camera
    Connectivity: IP/Network Wireless
    Supported Operating Systems: No
    High Definition: 1080P(Full-HD)
    Lens (mm): 3.6mm
    Installation: Side
    Power Supply(V): DC12V 2A
    Network Interface: RJ-45 10/100Mb Ethernet Slot
    Wall Bracket: Ceiling
    Supported Mobile Systems: Android
    Supported Mobile Systems: ios
    IR Distance(m): 30M-50M
    Special Features: Waterproof / Weatherproof
    Viewing Angle : 140°
    Megapixels: 2MP
    Power Consumption(W): 12W
    Storage: None
    Sensor Brand: SONY
    Alarm Action: Email Photo
    Alarm Action: Local Alarm
    Video Compression Format: H.264
    is_customized: Yes
    Technology: Pan Tilt Zoom
    Power Supply: Normal
    TF Card: None
    Color: White
    Audio Output: Two Way Audio
    Sensor: CMOS
    wifi camera: ip camera
    camara wifi: camara ip
    camera ip: camera wifi
    ip camera wifi: camara vigilancia exterior
    ptz: camara ip ptz

Surskriboj sur la etikedo:
WIFI CARAM
Model : WF-HD820-ZS
Résolution : 1080P
Power Supply : DC12V/1A
SSID : HSIPC-******
Password : 12345678
MAC : 00:88:09:**:**:**

Kunligante ĝin per Eterreto, ĝi akiras DHCP-adreson kaj ni povas konekti al la retejo:
defaŭlta uzanto: admin
defaŭlta pasvorto: admin

dum 5 minutoj, vi povas konekti per telnet al la haveno 9527
defaŭlta uzanto: root
defaŭlta pasvorto: jco66688
por resti konektita preter 5 minutoj : «killall -9 auto_run.sh»
(jco = http://www.cnjabsco.com/ )

malfermaj havenoj :  
  tcp : 80(http), 443(http ankaŭ !), 554(rtsp), 8004, 8006, 9527(telnetd), 9999  
  udp : 67(dhcp), 3702, 8002, 39642  

reta konduto kiam oni lanĉas jco_server:
pridemandas la servilon dns pri: www.163.com kaj dns.ictun.com
demando 114.114.114.114 por dns.ictun.com
konekti al la haveno udp/993 de dns.ictun.com
konekti al udp/993-haveno de 120.25.129.41
pridemandu la servilon dns pri: conn-policy.ictun.com
pridemandu la servilon dns pri: video-policy.ictun.com
konekti al udp/80-haveno de 52.29.45.252
konektas al udp/1210-haveno de 52.29.45.252
ligas al udp/21-haveno de 52.57.135.134
konektas al udp/1053-haveno de 52.28.62.20
konektas al udp/443-haveno de 52.29.58.225
konektas al udp/123 (ntp) haveno de 112.74.204.233 (video-policy.ictun.com)
ligas al udp/993-haveno de 47.52.99.182
konektas al udp/1054-haveno de 52.57.127.207
...
noto: ŝanĝu la defaŭltan itineron metante tiun de la interreta skatolo.

haltigi jco_server sen kaŭzi restartigi:
## stop_cloud :
ps | awk '/[a]uto_run.sh/ {print $1}' | while read PID; do kill -9 $PID; done;
ps | awk '/[j]co_server/ {print $1}' | xargs kill -9 &>/dev/null
echo 'V'>/dev/watchdog
echo 'V'>/dev/watchdog0
rm '/opt/media/mmcblk0p1/cid.txt' &>/dev/null

vi povas vidi la ĉefan video-rivereton (laŭ 1080P aŭ alia agordo) per:
ffplay -i rtsp://admin:admin@192.168.24.111/stream1
ffplay -i rtsp://admin:admin@192.168.24.39:554/stream1
ffplay -i rtsp://admin:admin@192.168.24.39:554/mpeg4/ch0/main/av_stream

kaj la duaranga :
ffplay -i rtsp://admin:admin@192.168.24.39/stream2
ffplay -i rtsp://admin:admin@192.168.24.39:554/stream2

la instalita firmvaro indikas:
DeviceName:     INGENIC-V01
DeviceModel:    IPCAM-100
DeviceSN:       12345678901
KernelVersion:  Thu Jul 9 15:46:54 CST 2015
ServerVersion:  WS3.53.1243-20190717-DANALE-GEN
WebVersion:     V2.3.4-20190709
OCXVersion:     -1

la procesoro estas : ingenic T21
http://www.ingenic.com.cn/en/?product/id/11.html
https://fr.indasina.com/ingenic-t21-extreme-2-megapixel-encoding-processor_p16.html

la CMOS-sensilo estas : gc2053.
ŝofora fonto : https://github.com/mrfixit2001/rk-kernel-fork/blob/develop-4.4/drivers/media/i2c/gc2053.c

RAM : 42MiB entute, 36 MiB havebla.

la haveno 9999 estas malfermita kaj permesas regi la fotilon:
echo "checkuser -act set -user admin -password admin" | nc 192.168.1.155 9999
echo "list" | nc 192.168.1.155 9999
echo "pelcod20ctrl -?" | nc 192.168.1.155 9999
echo "pelcod20ctrl -type 1" | nc 192.168.1.155 9999

gpio-havenoj:
46 = infraruĝaj LEDoj.
81 = bluaj LEDoj.

GPIO-havenoj blokitaj por studado:
blokita de motor.ko: 18? 38 39 40 41 47 48 49 60?
blokita de audio.ko: 63?
52 64

Ptz-kontrolo:

supro: http://admin:admin@192.168.24.39/?jcpcmd=pelcod20ctrl -type 1 -cmd 1 -data1 24 -data2 24
malsupre: http://admin:admin@192.168.24.39/?jcpcmd=pelcod20ctrl -type 1 -cmd 2 -data1 24 -data2 24
maldekstre: http://admin:admin@192.168.24.39/?jcpcmd=pelcod20ctrl -type 1 -cmd 3 -data1 24 -data2 24
dekstre: http://admin:admin@192.168.24.39/?jcpcmd=pelcod20ctrl -type 1 -cmd 4 -data1 24 -data2 24
halto: http://admin:admin@192.168.24.39/?jcpcmd=pelcod20ctrl -type 1 -cmd 9 -data1 24 -data2 24

starigu pozicion 2: http://192.168.24.39/?jcpcmd=pelcod20ctrl -type 2 -cmd 1 -data2 2
iru al 1: http://192.168.24.39/?jcpcmd=pelcod20ctrl -type 2 -cmd 2 -data2 1
forigi 2: http://192.168.24.39/?jcpcmd=pelcod20ctrl -type 2 -cmd 3 -data2 2



