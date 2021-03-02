

# komencante

ni kreas /opt/etc/local.rc, kio estas komencita preter la originala firmvaro.
local.rc lanĉas run.sh el la SDa karto.

# run.sh :

 * malkonektas la reton 
 * atendas 16 sekundoj, por jco_server pravalorizi la fotilon
 * haltigas jco_server kaj autorun.sh.
 * malebligas /dev/watchdog kaj /dev/watchdog0
 * pravalorizas LEDojn kaj motoron
 * pravalorizas la etc-dosierujon se necese
 * redirektas kaj /bin/busybox kaj /etc kaj /root al la SDa karto.
 * pravalorizas la CRONTABo se necese
 * komencitas crond-servon, reton, ntpd-servon, reteja-servon
 * komencitas enabligajn servojn (en config/autostart)
 * komencitas uzantajn skriptojn (en config/userscripts)
