#!/bin/sh

echo "Content-type: application/json"
echo "Pragma: no-cache"
echo "Cache-Control: max-age=0, no-store, no-cache"
echo

. ${DOCUMENT_ROOT}/../scripts/common_functions.sh

poz=$(cat /tmp/motoraValoro)
echo -n $poz
