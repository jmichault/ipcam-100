#!/bin/sh                 
. /etc/profile >/dev/null 2>&1

. ${SDCARD}/www/cgi-bin/func.cgi

#source ${SDCARD}/scripts/common_functions.sh
    
echo "Content-type: text"
echo "Pragma: no-cache"
echo "Cache-Control: max-age=0, no-store, no-cache"
echo ""

if [ -n "$F_cmd" ]; then
  case "$F_cmd" in
   getDirs)
    find "/opt/media/mmcblk0p1/motio/${F_dir}/" -type d -maxdepth 1 -mindepth 1|sort -r|while read file
      do
        file_name=$(basename "$file")
        file_count=$(find "$file" -type f|wc -l|awk '{print $1;}')
        echo "${file_name}#:#${file_count}"
      done
    return
    ;;
   getFiles)
    find "/opt/media/mmcblk0p1/motio/${F_dir}/${F_subdir}" -type f -maxdepth 1 -mindepth 1|sort -r|while read file
      do                                    
        file_url="$file"                                                                                        
        shift $#                                                                                                  
        set -- $(ls -lh "$file")                                                                                  
        shift 4                                                                                                   
        file_size=$1                                                                                            
        shift                                                                                                     
        file_date="$1 $2 $3"                                                                                    
        file_name=${file##?*/}                                                                                  
        echo "${file_name}#:#${file_size}#:#${file_date}#:#${file_url}"
      done
    return
    ;;
   *)
    echo "Unsupported command '$F_cmd'"
    ;;

  esac
fi
