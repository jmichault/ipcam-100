#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "agordolegilo.h"

extern char * AgordoVojo;

struct sensilo_agordo SenAgordo=
{
  .cbus_type=1, // 1=I2C , 2=SPI
  .name="gc2053",
  .addr=55,
  .width=1920,
  .heigth=1080
};

struct kanalo_agordo KanAgordo[2]=
{
 { .picWidth=1920,
   .picHeight=1080,
   .format=3, // -1=MJPEG, 0=FixQP, 1=CBR, 2=VBR, 3=Smart, 4=INV
   .minQP=15,
   .maxQP=48,
   .bitrate=600,
   .fpnum=12,
 },
 { .picWidth=640,
   .picHeight=360,
   .format=-1,
   .minQP=20,
   .maxQP=48,
   .bitrate=512,
   .fpnum=10,
 }
};

bool AgordoNeedReset;

static void doTrt( char * nomo, char * valoro)
{
  if(!strcmp(nomo,"sensor.type"))
  {
    if( !strcmp(valoro,"I2C") ) SenAgordo.cbus_type=1;
    else if( !strcmp(valoro,"SPI") ) SenAgordo.cbus_type=2;
    else
      fprintf(stderr,"ne kognata sensor.type %s\n",valoro);
  }
  else if(!strcmp(nomo,"sensor.nom"))
    strncpy(SenAgordo.name,valoro,32);
  else if(!strcmp(nomo,"sensor.largeur"))
    SenAgordo.width = strtol(valoro,NULL,10);
  else if(!strcmp(nomo,"sensor.hauteur"))
    SenAgordo.heigth = strtol(valoro,NULL,10);
  else if(!strcmp(nomo,"sensor.adresse"))
    SenAgordo.addr = strtol(valoro,NULL,10);
  else if(!strcmp(nomo,"canal[1].taille"))
  {
    short width,height;
    sscanf(valoro,"%hdx%hd",&width,&height);
    if(width != KanAgordo[0].picWidth || height != KanAgordo[0].picHeight) AgordoNeedReset=true;
    KanAgordo[0].picWidth=width;
    KanAgordo[0].picHeight=height;
  }
  else if(!strcmp(nomo,"canal[1].format"))
  { // -1=MJPEG, 0=FixQP, 1=CBR, 2=VBR, 3=Smart, 4=INV
    if     ( !strcmp(valoro,"MJPEG") ) KanAgordo[0].format=-1;
    else if( !strcmp(valoro,"FixQP") ) KanAgordo[0].format= 0;
    else if( !strcmp(valoro,  "CBR") ) KanAgordo[0].format= 1;
    else if( !strcmp(valoro,  "VBR") ) KanAgordo[0].format= 2;
    else if( !strcmp(valoro,"Smart") ) KanAgordo[0].format= 3;
    else if( !strcmp(valoro,  "INV") ) KanAgordo[0].format= 4;
    else
      fprintf(stderr,"ne kognata canal[1].format %s\n",valoro);
  }
  else if(!strcmp(nomo,"canal[1].minqp"))
    KanAgordo[0].minQP = strtol(valoro,NULL,10);
  else if(!strcmp(nomo,"canal[1].maxqp"))
    KanAgordo[0].maxQP = strtol(valoro,NULL,10);
  else if(!strcmp(nomo,"canal[1].bitrate"))
    KanAgordo[0].bitrate = strtol(valoro,NULL,10);
  else if(!strcmp(nomo,"canal[1].fps"))
    KanAgordo[0].fpnum = strtol(valoro,NULL,10);
  else if(!strcmp(nomo,"canal[2].taille"))
  {
    short width,height;
    sscanf(valoro,"%hdx%hd",&width,&height);
    if(width != KanAgordo[1].picWidth || height != KanAgordo[1].picHeight) AgordoNeedReset=true;
    KanAgordo[1].picWidth=width;
    KanAgordo[1].picHeight=height;
  }
  else if(!strcmp(nomo,"canal[2].format"))
  { // -1=MJPEG, 0=FixQP, 1=CBR, 2=VBR, 3=Smart, 4=INV
    if     ( !strcmp(valoro,"MJPEG") ) KanAgordo[1].format=-1;
    else if( !strcmp(valoro,"FixQP") ) KanAgordo[1].format= 0;
    else if( !strcmp(valoro,  "CBR") ) KanAgordo[1].format= 1;
    else if( !strcmp(valoro,  "VBR") ) KanAgordo[1].format= 2;
    else if( !strcmp(valoro,"Smart") ) KanAgordo[1].format= 3;
    else if( !strcmp(valoro,  "INV") ) KanAgordo[1].format= 4;
    else
      fprintf(stderr,"ne kognata canal[2].format %s\n",valoro);
  }
  else if(!strcmp(nomo,"canal[2].minqp"))
    KanAgordo[1].minQP = strtol(valoro,NULL,10);
  else if(!strcmp(nomo,"canal[2].maxqp"))
    KanAgordo[1].maxQP = strtol(valoro,NULL,10);
  else if(!strcmp(nomo,"canal[2].bitrate"))
    KanAgordo[1].bitrate = strtol(valoro,NULL,10);
  else if(!strcmp(nomo,"canal[2].fps"))
    KanAgordo[1].fpnum = strtol(valoro,NULL,10);
  else
   fprintf(stderr,"ne kognata agordo %s\n",nomo);
}


void agordoLegilo()
{
FILE *ficin;
char buffer[81];
char bufferValue[81];
int ret;
int lastret=0;
  if ( !AgordoVojo)
  {
    ficin = fopen("/opt/media/mmcblk0p1/www/ipcam/config/rtsp.conf","r");
  }
  else
  {
    sprintf(buffer,"%s/rtsp.conf",AgordoVojo);
    ficin = fopen(buffer,"r");
  }
  AgordoNeedReset=false;
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
