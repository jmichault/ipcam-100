/* 
 * Copyright (c) 2021 Jean Michault.
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <imp/imp_ivs.h>
#include <imp/imp_ivs_move.h>
#include "movolegilo.h"

extern IMP_IVS_MoveParam MovoParam;
extern char * AgordoVojo;
struct S_movAgordo movAgordo=
{ false,3,15,2,7
 ,false,false
 ,false,NULL
 ,false,NULL
 ,false,NULL
 ,false,NULL
};

// var config=[{"id":0,"x":4,"y":161,"z":0,"height":70,"width":67}, ...
// var DetektoDatenoj=[{"name":"on","value":"1"},{"name":"sentemo","value":"2"}...


static void doTrt( char * nomo, char * valoro)
{
  if(!strcmp(nomo,"on"))
    movAgordo.Aktivida = strtol(valoro,NULL,10);
  else if(!strcmp(nomo,"sentemo"))
    movAgordo.Sentemo = strtol(valoro,NULL,10);
  else if(!strcmp(nomo,"FinaTempo"))
    movAgordo.FinaTempo = strtol(valoro,NULL,10);
  else if(!strcmp(nomo,"SenaktivaTempo"))
    movAgordo.SenaktivaTempo = strtol(valoro,NULL,10);
  else if(!strcmp(nomo,"MinZonoj"))
    movAgordo.MinZonoj = strtol(valoro,NULL,10);
  else if(!strcmp(nomo,"MaxZonoj"))
    movAgordo.MaxZonoj = strtol(valoro,NULL,10);
  else if(!strcmp(nomo,"StokPicOn"))
    movAgordo.StokPicOn = strtol(valoro,NULL,10);
  else if(!strcmp(nomo,"StokMovOn"))
    movAgordo.StokMovOn = strtol(valoro,NULL,10);
  else if(!strcmp(nomo,"SciiMailOn"))
    movAgordo.SciiMailOn = strtol(valoro,NULL,10);
  else if(!strcmp(nomo,"SciiMailAl"))
    movAgordo.SciiMailAl = strdup(valoro);
  else if(!strcmp(nomo,"SciiTelOn"))
    movAgordo.SciiTelOn = strtol(valoro,NULL,10);
  else if(!strcmp(nomo,"SciiTelAl"))
    movAgordo.SciiTelAl = strdup(valoro);
  else if(!strcmp(nomo,"SciiUrlOn"))
    movAgordo.SciiUrlOn = strtol(valoro,NULL,10);
  else if(!strcmp(nomo,"SciiUrlUrl"))
    movAgordo.SciiUrlUrl = strdup(valoro);
  else if(!strcmp(nomo,"SciiOrdOn"))
    movAgordo.SciiOrdOn = strtol(valoro,NULL,10);
  else if(!strcmp(nomo,"SciiOrdOrd"))
    movAgordo.SciiOrdOrd = strdup(valoro);
  else
  {
    fprintf(stderr," Mova agordo ne kognata : nomo=%s ; valoro=%s\n",nomo,valoro);
  }
}

void movoLegilo()
{
FILE *ficin;
char buffer[81];
char bufferValue[81];
int ret;
int lastret=0;
int nbRect=0;
  if ( !AgordoVojo)
  {
    ficin = fopen("/opt/media/mmcblk0p1/www/ipcam/config/movo.conf","r");
  }
  else
  {
    sprintf(buffer,"%s/movo.conf",AgordoVojo);
    ficin = fopen(buffer,"r");
  }
  for(ret=0 ; ret!= EOF ; )
  {
    ret=fgetc(ficin);
    if(ret == EOF) break;
    // search for {"id:"
    if(ret =='{' )
    {
     int id=-1 , x=-1 , y=-1 , z=-1 , h=-1 , w=-1;
      ret=fscanf(ficin,"\"id\":%d,\"x\":%d,\"y\":%d,\"z\":%d,\"height\":%d,\"width\":%d}"
		,&id, &x, &y, &z, &h, &w);
      if (ret<=0) break;
      MovoParam.roiRect[nbRect].p0.x = x/2;
      MovoParam.roiRect[nbRect].p0.y = y/2;
      MovoParam.roiRect[nbRect].p1.x = (x+w)/2;
      MovoParam.roiRect[nbRect].p1.y = (y+h)/2;
      MovoParam.sense[nbRect] = 4;
      nbRect++;
    }
  }
  if(nbRect) MovoParam.roiRectCnt=nbRect;
  for(lastret=0 ; ret!= EOF ; lastret=ret)
  {
    ret=fgetc(ficin);
    if(ret == EOF) break;
    if(ret =='n' && ! isalnum(lastret))
    {
      long fpos1=ftell(ficin);
      fscanf(ficin,"ame\":\"");
      long fpos2=ftell(ficin);
      if (fpos2 != (fpos1+strlen("ame\":\"")))
        continue;
      int pos;
      for ( pos= 0 ; pos<80 ; pos++)
      {
        ret=fgetc(ficin);
        if(ret== '"' && lastret != '\\') break;
        buffer[pos]= (char) ret;
        lastret=ret;
      }
      buffer[pos]=0;
      if(pos==80 && ret != '"')
      {
        fprintf(stderr,"string too long at %ld.\n",fpos2);
        continue;
      }
      fpos1=ftell(ficin);
      fscanf(ficin,",\"value\":\"");
      fpos2=ftell(ficin);
      if (fpos2 != (fpos1+strlen(",\"value\":\"")))
        continue;
      for ( pos= 0 ; pos<80 ; pos++)
      {
        ret=fgetc(ficin);
        if(ret== '"' && lastret != '\\') break;
        bufferValue[pos]= (char) ret;
        lastret=ret;
      }
      bufferValue[pos]=0;
      if(pos==80 && ret != '"')
      {
        fprintf(stderr,"string too long at %ld.\n",fpos2);
        continue;
      }
      doTrt(buffer,bufferValue);
    }
  }
  fclose(ficin);
}
