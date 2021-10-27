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

#include <stdlib.h>
#include <string.h>
#ifdef UZI_DMALLOC
#include <dmalloc.h>
#endif
#include <BasicUsageEnvironment/BasicUsageEnvironment.hh>
#include <GroupsockHelper.hh> // for "weHaveAnIPv*Address()"
#include <signal.h>
#include "DynamicRTSPServer.hh"
#include "imp_komuna.h"
#include "agordolegilo.h"
#include "movolegilo.h"

char * AgordoVojo=NULL;

UsageEnvironment* env = NULL;
RTSPServer* rtspServer = NULL;
TaskScheduler* scheduler = NULL;
UserAuthenticationDatabase* authDB = NULL;

char quit = 0;

void sighandler(int n) {
  printf("ricevita signalo %d\n",n);
#ifdef UZI_DMALLOC
  dmalloc_log_stats();
#endif
  quit = 1;
  sleep(5);
  Medium::close(rtspServer);
  delete scheduler;
  printf("avant env->reclaim\n");
  if (env->reclaim()) printf("env->reclaim() OK\n");
  else printf("env->reclaim() KO\n");
  delete scheduler;
  delete authDB;
  imp_exit();
#ifdef UZI_DMALLOC
  printf("avant dmalloc_shutdown\n");
  dmalloc_shutdown ();
#endif
  exit(0);
}



int main(int argc, char** argv) {
  
  if(argc>1) AgordoVojo=argv[1];
  // Begin by setting up our usage environment:
  scheduler = BasicTaskScheduler::createNew();
  env = BasicUsageEnvironment::createNew(*scheduler);

  UserAuthenticationDatabase* authDB = NULL;
/*
  // To implement client access control to the RTSP server, do the following:
  authDB = new UserAuthenticationDatabase;
  authDB->addUserRecord("admin", "ismart21"); // replace these with real strings
*/
/*
  // 
  authDB = new UserAuthenticationDatabase("RTSP server",true);
  // echo -n "admin:RTSP server:ismart21"|busybox md5sum
  // d05b52a484376265b08c79fc81f972fa
  authDB->addUserRecord("admin", "d05b52a484376265b08c79fc81f972fa");
  // Repeat the above with each <username>, <password> that you wish to allow
  // access to the server.
*/
  authDB = new UserAuthenticationDatabase("RTSP server",true);
  {
    FILE * ficin = fopen("/opt/media/mmcblk0p1/config/rtsp.user","r");
    char buffer[150];
    if(ficin)
    {
      while(fgets(buffer,149,ficin))
      {
        char * pCol=strstr(buffer,":");
        if(pCol)
        {
          (*pCol)=0; pCol++;
          char * pLn=strstr(pCol,"\n");
          if (pLn) (*pLn)=0;
          authDB->addUserRecord(buffer, pCol);
        }
      }
      fclose(ficin);
    }
    else
    {
      fprintf(stderr,"  rtsp.user ne trovita.\n");
      authDB->addUserRecord("admin","ismart21"); 
    }
  }

  agordoLegilo();
  movoLegilo();
  if(KanAgordo[0].picWidth != fs_chn_attrs[0].picWidth
	|| KanAgordo[0].picHeight != fs_chn_attrs[0].picHeight)
  {
    fs_chn_attrs[0].picWidth = KanAgordo[0].picWidth;
    fs_chn_attrs[0].picHeight = KanAgordo[0].picHeight;
    if(KanAgordo[0].picWidth != 1920 || KanAgordo[0].picHeight != 1080 ) // sensorwidth , sensorheight
      fs_chn_attrs[0].scaler.enable=1;
    else
      fs_chn_attrs[0].scaler.enable=0;
    fs_chn_attrs[0].scaler.outwidth = KanAgordo[0].picWidth;
    fs_chn_attrs[0].scaler.outheight = KanAgordo[0].picHeight;
    channel_attrs[0].encAttr.picWidth = KanAgordo[0].picWidth;
    channel_attrs[0].encAttr.picHeight = KanAgordo[0].picHeight;
    channel_attrs[0].xW = KanAgordo[0].picWidth;
    channel_attrs[0].xH = KanAgordo[0].picHeight;
  }
  if(KanAgordo[1].picWidth != fs_chn_attrs[1].picWidth
	|| KanAgordo[1].picHeight != fs_chn_attrs[1].picHeight)
  {
    fs_chn_attrs[1].picWidth = KanAgordo[1].picWidth;
    fs_chn_attrs[1].picHeight = KanAgordo[1].picHeight;
    if(KanAgordo[1].picWidth != 1920 || KanAgordo[1].picHeight != 1080 ) // sensorwidth , sensorheight
      fs_chn_attrs[1].scaler.enable=1;
    else
      fs_chn_attrs[0].scaler.enable=0;
    fs_chn_attrs[1].scaler.outwidth = KanAgordo[1].picWidth;
    fs_chn_attrs[1].scaler.outheight = KanAgordo[1].picHeight;
    channel_attrs[1].encAttr.picWidth = KanAgordo[1].picWidth;
    channel_attrs[1].encAttr.picHeight = KanAgordo[1].picHeight;
    channel_attrs[1].xW = KanAgordo[1].picWidth;
    channel_attrs[1].xH = KanAgordo[1].picHeight;
  }
  if (SenAgordo.cbus_type==1) sensor_info.cbus_type = TX_SENSOR_CONTROL_INTERFACE_I2C;
  else if (SenAgordo.cbus_type==2) sensor_info.cbus_type = TX_SENSOR_CONTROL_INTERFACE_SPI;
  if (SenAgordo.name[0])
  {
    strncpy(sensor_info.name,SenAgordo.name,32);
    strncpy(sensor_info.i2c.type,SenAgordo.name,32);
  }
  if (SenAgordo.addr)
  {
    if ( sensor_info.cbus_type == TX_SENSOR_CONTROL_INTERFACE_I2C)
      sensor_info.i2c.addr = SenAgordo.addr;
    else
      sensor_info.spi.bus_num = SenAgordo.addr;
  }
  fs_chn_attrs[0].outFrmRateNum = KanAgordo[0].fpnum;
  channel_attrs[0].rcAttr.outFrmRate.frmRateNum = KanAgordo[0].fpnum;
  fs_chn_attrs[1].outFrmRateNum = KanAgordo[1].fpnum;
  channel_attrs[1].rcAttr.outFrmRate.frmRateNum = KanAgordo[1].fpnum;
  if (KanAgordo[0].format <0)
  { // ŝanĝas agordojn de kodigilo al mjpg
    channel_attrs[0].encAttr.enType=PT_JPEG;
    channel_attrs[0].encAttr.profile=0;
  }
  else
  {
    channel_attrs[0].encAttr.enType=PT_H264;
    channel_attrs[0].encAttr.profile=2;
    channel_attrs[0].rcAttr.attrRcMode.rcMode = (IMPEncoderRcMode)KanAgordo[0].format;
    channel_attrs[0].rcAttr.attrRcMode.attrH264Vbr.maxQp = KanAgordo[0].maxQP;
    if (   channel_attrs[0].rcAttr.attrRcMode.rcMode == ENC_RC_MODE_SMART
             || channel_attrs[0].rcAttr.attrRcMode.rcMode == ENC_RC_MODE_VBR)
    {
      channel_attrs[0].rcAttr.attrRcMode.attrH264Vbr.minQp = KanAgordo[0].minQP;
      channel_attrs[0].rcAttr.attrRcMode.attrH264Vbr.maxBitRate = KanAgordo[0].bitrate;
      channel_attrs[0].rcAttr.attrRcMode.attrH264Vbr.staticTime = 2;
      channel_attrs[0].rcAttr.attrRcMode.attrH264Vbr.iBiasLvl = 0;
      channel_attrs[0].rcAttr.attrRcMode.attrH264Vbr.changePos = 80;
      channel_attrs[0].rcAttr.attrRcMode.attrH264Vbr.qualityLvl = 2;
      channel_attrs[0].rcAttr.attrRcMode.attrH264Vbr.frmQPStep = 3;
      channel_attrs[0].rcAttr.attrRcMode.attrH264Vbr.gopQPStep = 15;
      channel_attrs[0].rcAttr.attrRcMode.attrH264Vbr.gopRelation = 0;

    }
    else if (channel_attrs[0].rcAttr.attrRcMode.rcMode == ENC_RC_MODE_CBR)
    {
      channel_attrs[0].rcAttr.attrRcMode.attrH264Cbr.minQp = KanAgordo[0].minQP;
      channel_attrs[0].rcAttr.attrRcMode.attrH264Cbr.outBitRate = KanAgordo[0].bitrate;
      channel_attrs[0].rcAttr.attrRcMode.attrH264Cbr.iBiasLvl = 0;
      channel_attrs[0].rcAttr.attrRcMode.attrH264Cbr.frmQPStep = 3;
      channel_attrs[0].rcAttr.attrRcMode.attrH264Cbr.gopQPStep = 15;
      channel_attrs[0].rcAttr.attrRcMode.attrH264Cbr.adaptiveMode = 1;
      channel_attrs[0].rcAttr.attrRcMode.attrH264Cbr.gopRelation = 0;
    }
  }
  if (KanAgordo[1].format <0)
  { // ŝanĝas agordojn de kodigilo al mjpg
    channel_attrs[1].encAttr.enType=PT_JPEG;
    channel_attrs[1].encAttr.profile=0;
  }
  else
  {
    channel_attrs[1].encAttr.enType=PT_H264;
    channel_attrs[1].encAttr.profile=2;
    channel_attrs[1].rcAttr.attrRcMode.rcMode = (IMPEncoderRcMode)KanAgordo[1].format;
    channel_attrs[1].rcAttr.attrRcMode.attrH264Vbr.maxQp = KanAgordo[1].maxQP;
    if (   channel_attrs[1].rcAttr.attrRcMode.rcMode == ENC_RC_MODE_SMART
             || channel_attrs[1].rcAttr.attrRcMode.rcMode == ENC_RC_MODE_VBR)
    {
      channel_attrs[1].rcAttr.attrRcMode.attrH264Vbr.minQp = KanAgordo[1].minQP;
      channel_attrs[1].rcAttr.attrRcMode.attrH264Vbr.maxBitRate = KanAgordo[1].bitrate;
      channel_attrs[1].rcAttr.attrRcMode.attrH264Vbr.staticTime = 2;
      channel_attrs[1].rcAttr.attrRcMode.attrH264Vbr.iBiasLvl = 0;
      channel_attrs[1].rcAttr.attrRcMode.attrH264Vbr.changePos = 80;
      channel_attrs[1].rcAttr.attrRcMode.attrH264Vbr.qualityLvl = 2;
      channel_attrs[1].rcAttr.attrRcMode.attrH264Vbr.frmQPStep = 3;
      channel_attrs[1].rcAttr.attrRcMode.attrH264Vbr.gopQPStep = 15;
      channel_attrs[1].rcAttr.attrRcMode.attrH264Vbr.gopRelation = 0;

    }
    else if (channel_attrs[1].rcAttr.attrRcMode.rcMode == ENC_RC_MODE_CBR)
    {
      channel_attrs[1].rcAttr.attrRcMode.attrH264Cbr.minQp = KanAgordo[1].minQP;
      channel_attrs[1].rcAttr.attrRcMode.attrH264Cbr.outBitRate = KanAgordo[1].bitrate;
      channel_attrs[1].rcAttr.attrRcMode.attrH264Cbr.iBiasLvl = 0;
      channel_attrs[1].rcAttr.attrRcMode.attrH264Cbr.frmQPStep = 3;
      channel_attrs[1].rcAttr.attrRcMode.attrH264Cbr.gopQPStep = 15;
      channel_attrs[1].rcAttr.attrRcMode.attrH264Cbr.adaptiveMode = 1;
      channel_attrs[1].rcAttr.attrRcMode.attrH264Cbr.gopRelation = 0;
    }
  }
printf("avant imp_init()\n");
  // init T21
  int ret=imp_init();
  if(ret<0)
  {
    fprintf(stderr,"imp_init failed\n");
    exit(1);
  }
printf("apres imp_init()\n");



  // Create the RTSP server.  Try first with the default port number (554),
  // and then with the alternative port number (8554):
  portNumBits rtspServerPortNum = 554;
  rtspServer = DynamicRTSPServer::createNew(*env, rtspServerPortNum, authDB);
  if (rtspServer == NULL) {
    rtspServerPortNum = 8554;
    rtspServer = DynamicRTSPServer::createNew(*env, rtspServerPortNum, authDB);
  }
  if (rtspServer == NULL) {
    *env << "Failed to create RTSP server: " << env->getResultMsg() << "\n";
    exit(1);
  }

  *env << "T21-hacks RTSP Server\n";

  *env << "Play streams from this server using the URL\n";
  if (weHaveAnIPv4Address(*env)) {
    char* rtspURLPrefix = rtspServer->ipv4rtspURLPrefix();
    *env << "\t" << rtspURLPrefix << "stream1\n";
    *env << "\t" << rtspURLPrefix << "stream2\n";
    delete[] rtspURLPrefix;
    if (weHaveAnIPv6Address(*env)) *env << "or\n";
  }
  if (weHaveAnIPv6Address(*env)) {
    char* rtspURLPrefix = rtspServer->ipv6rtspURLPrefix();
    *env << "\t" << rtspURLPrefix << "stream1\n";
    *env << "\t" << rtspURLPrefix << "stream2\n";
    delete[] rtspURLPrefix;
  }


  // Also, attempt to create a HTTP server for RTSP-over-HTTP tunneling.
  // Try first with (8080), and then with the alternative HTTP
  // port numbers (8000 and 8080).

  if (rtspServer->setUpTunnelingOverHTTP(8000) || rtspServer->setUpTunnelingOverHTTP(8080)) {
    *env << "(We use port " << rtspServer->httpServerPortNum() << " for optional RTSP-over-HTTP tunneling, or for HTTP live streaming (for indexed Transport Stream files only).)\n";
  } else {
    *env << "(RTSP-over-HTTP tunneling is not available.)\n";
  }
  signal(SIGTERM|SIGINT|SIGQUIT, sighandler);

  env->taskScheduler().doEventLoop(); // does not return
  *env << "T21-hacks , foriri RTSP Server\n";

  return 0;
}
