#!/usr/bin/env bash
################################################################
# skripto por aŭtomate traduki frazon
################################################################

src="$1"
dst="$2"
txt="$3"

cook=`find _traduko.jar -mmin -15 2>/dev/null`

PROVO=0
URL=0

while [ $PROVO -lt 3 ]
do

if [ x$cook = x ]
then
  curl -c _traduko.jar -A 'Mozilla/5.0 (X11; Linux x86_64; rv:68.0) Gecko/20100101 Firefox/68.0' \
   "https://translate.google.com" -o /dev/null 2>/dev/null
fi

#URLS=("translate.google.com" "translate.google.co.kr" "translate.google.ac" "translate.google.ad" "translate.google.ae")

# proxies = {    'http': '1.243.64.63:48730',   'https': '59.11.98.253:42645', }
#echo URL=$URL
#SITE=${URLS[$URL]}
#echo SITE=$SITE
SITE=translate.google.com

MSG0=$(curl -b _traduko.jar -A 'Mozilla/5.0 (X11; Linux x86_64; rv:88.0) Gecko/20100101 Firefox/88.0' \
 --refer "https://${SITE}/" \
 "https://${SITE}/translate_a/single?client=webapp&sl=${src}&tl=${dst}&hl=${dst}&dt=at&dt=bd&dt=ex&dt=ld&dt=md&dt=qca&dt=rw&dt=rm&dt=ss&dt=t&dt=gt&pc=1&otf=1&ssel=0&tsel=0&kc=1&tk=&ie=UTF-8&oe=UTF-8" \
--data-urlencode "q=${txt}" 2>/dev/null \
)

#echo MSG0=$MSG0
#if echo "$MSG0"|grep "sorry" >/dev/null
#then
#  echo sorry
#  URL=$(($URL+1))
#rm _traduko.jar
#echo URL=$URL
#  continue
#else
#  echo pas sorry
#fi
#exit

if echo "$MSG0"|grep "sorry" >/dev/null
then
 #echo sorry
  BASEDIR=$(dirname $(readlink -f $0))
  MSG=$("$BASEDIR"/trans -b -s $src -t $dst "$txt" )
else
# echo pas sorry
  MSG=$(echo $MSG0 \
  |sed "s/\\\\u003d/=/g;s/\\\\u003c/</g;s/\\\\u003e/>/g" \
  |sed "s/\\\\u200b/\xe2\x80\x8b/g" \
  |grep "\",null,null,[0-9]"|egrep -v "\[[1-9],"|sed "s/\",\".*//;s/^[,\[]*\"//" \
  | tr -d "\n" \
  | sed "s/\\\ [nN]/n/g;s/] (/](/g;s/ __ / __/g" \
  | sed "s/\. \\\n$/.  \\\n/" \
)
fi

if [ "x$MSG" = "x" ]
then
  cook=""
else
  echo -n "$MSG"
  break;
fi
  PROVO=$(($PROVO+1))
done
 
