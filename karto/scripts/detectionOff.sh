#!/bin/sh

. ${SDCARD}/scripts/common_functions.sh

.  JsAlVar ${SDCARD}/www/ipcam/config/movo.conf DetektoDatenoj

# Run user script.
if [ x"$SciiOrdOn" == x1 ]
then
  $SciiOrdOrd
fi
