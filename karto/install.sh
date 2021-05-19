#!/bin/sh

SDCARD=`pwd`

DST_PATCH="/opt/etc/local.rc"
SRC_PATCH="${SDCARD}/patch/local.rc"
AUTORUN_FILE="/ipc/etc/auto_run.sh"

. ${SDCARD}/scripts/common_functions.sh

apply_patch()
{
    cp -f $SRC_PATCH $DST_PATCH
    if [ $? -ne 0 ] ; then
        echo "ERROR: can't write patch file to $DST_PATCH"
        echo "ERROR: patch not installed!"
    else
        chmod +x $DST_PATCH
        sync
        echo "SUCCESS install patch!"
        sleep 5
	# mount...
	#echo "PLEASE change root password :"
	#passwd
        reboot
    fi
}

############################################################
echo "=====Installing patch====="

if is_file_contain_str "$AUTORUN_FILE" "$DST_PATCH"; then
    echo "System check success."
    apply_patch
else
    echo "WARNING: found unsupported system version."
    while true; do
        read -p "Do you wish to force apply this patch?[y/n Default:y]: " yn
        case $yn in
            [Yy]* ) apply_patch; break;;
            [Nn]* ) echo "Patch not applied"; break;;
            * ) apply_patch;break;;
        esac
    done
fi


