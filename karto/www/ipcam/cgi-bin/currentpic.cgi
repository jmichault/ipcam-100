#!/bin/sh
. /etc/profile >/dev/null 2>&1

echo "Content-type: image/jpeg"
echo ""
${DOCUMENT_ROOT}/../bin/getimage
