#!/bin/sh

echo "Content-type: image/jpeg"
echo ""
${SDCARD}/bin/getimage |  ${SDCARD}/bin/jpegtran -progressive -optimize

