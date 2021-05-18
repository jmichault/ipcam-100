#!/bin/sh

echo "Content-type: image/jpeg"
echo ""
${DOCUMENT_ROOT}/../bin/getimage |  ${DOCUMENT_ROOT}/../bin/jpegtran -progressive -optimize

