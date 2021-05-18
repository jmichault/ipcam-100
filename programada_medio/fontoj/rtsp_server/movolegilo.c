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

// var config=[{"id":0,"x":4,"y":161,"z":0,"height":70,"width":67}, ...

void movoLegilo()
{
FILE *ficin = fopen("/opt/media/mmcblk0p1/www/config/movo.conf","r");
char buffer[81];
char bufferValue[81];
int ret;
int nbRect=0;
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
      if (ret<0) break;
      fprintf(stderr," movelegilo : id=%d,x=%d,y=%d,w=%d,h=%d\n",id,x,y,w,h);
      MovoParam.roiRect[nbRect].p0.x = x/2;
      MovoParam.roiRect[nbRect].p0.y = y/2;
      MovoParam.roiRect[nbRect].p1.x = (x+w)/2;
      MovoParam.roiRect[nbRect].p1.y = (y+h)/2;
      MovoParam.sense[nbRect] = 4;
      nbRect++;
    }
  }
  if(nbRect) MovoParam.roiRectCnt=nbRect;
  for(ret=0 ; ret!= EOF ; )
  {
    ret=fgetc(ficin);
    if(ret == EOF) break;
    // search for {"id:"
    if(ret =='{' )
    {
    }
  }
}
