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
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#include <imp/imp_log.h>
#include <imp/imp_ivs.h>
#include <imp/imp_ivs_move.h>
#include <imp/imp_osd.h>

#define IMP_LOG IMP_LOG_TO_FILE
#define IMP_LOG_OUT_DEFAULT IMP_LOG_OUT_STDOUT
#define TAG "imp_komuna"
#include "imp_komuna.h"
#include "../rtspserver-tools/sharedmem.h"

//param
IMP_IVS_MoveParam MovoParam=
{
  .skipFrameCnt = 5,
  .roiRectCnt=4,
  .frameInfo.width = 320,
  .frameInfo.height = 184,
  .sense = {2,2,2,2},
  .roiRect[0]=
	{ .p0.x =   0, .p0.y =   0, .p1.x = 160, .p1.y =  92},
  .roiRect[1]=
	{ .p0.x =   0, .p0.y =  92, .p1.x = 160, .p1.y = 184},
  .roiRect[2]=
	{ .p0.x = 160, .p0.y =   0, .p1.x = 320, .p1.y =  92},
  .roiRect[3]=
	{ .p0.x = 160, .p0.y =  92, .p1.x = 320, .p1.y = 184},
}; 

static void *ekrankopio_process(void *arg)
{
  int i = 0, ret = 0;
  printf(" ekrankopio_process komenco\n");
  SharedMem_init();
  int snap_num=-1;
  if( channel_attrs[0].encAttr.enType==PT_JPEG) snap_num=0;
  else if( channel_attrs[1].encAttr.enType==PT_JPEG) snap_num=1;

  for (i = 0;  ; i++)
  {
    if( snap_num>=0)
    {
      /* JPEG Snap */
      int ret = IMP_Encoder_PollingStream(snap_num, 1000);
      if (ret >= 0)
      {
        IMPEncoderStream stream;
        int bytesRead=0;
        static unsigned char *bufferStream = NULL;
        if(!bufferStream) bufferStream = (unsigned char *) malloc(1920*1080);
        /* Get JPEG Snap */
        ret = IMP_Encoder_GetStream(snap_num, &stream, 1);
        if (ret >= 0)
        {
          int nr_pack = stream.packCount;
          void *memoryAddress = (void *)bufferStream;
          bytesRead = 0;
          for (int i = 0; i < nr_pack; i++)
          {
            int packLen = stream.pack[i].length;
            memcpy(memoryAddress, (void *) stream.pack[i].virAddr, packLen);
            memoryAddress = (void *) ((char *) memoryAddress + packLen);
            bytesRead = bytesRead + packLen;
          }
  
        }
        IMP_Encoder_ReleaseStream(snap_num, &stream);
        SharedMem_copyImage(bufferStream,bytesRead);
      }
    }

    usleep(1000000);
  }
  printf("get_result_process fino.\n");

  return (void *)0;
}


char * detectionScriptOn = "/opt/media/sdc/scripts/detectionOn.sh";
char * detectionScriptOff = "/opt/media/sdc/scripts/detectionOff.sh";

static int file_exist(const char *filename)
{
  FILE *f = fopen(filename,"r");
  if (f == NULL)
    return 0;
  fclose(f);
  return 1;
}

static void exec_command(const char *command, char param[4][2])
{
  if (file_exist(command))
  {
    if (param == NULL)
    {
      int retVal =  system(command);
    }
    else
    {
      char exe[256] = {0};
      snprintf(exe, sizeof(exe), "%s %s %s %s %s", command, param[0],param[1],param[2],param[3]);
      int retVal =  system(exe);
    }
  }
  else
  {
         printf("komando «%s» ne ekzistas\n",command);
  }
}


static void *ivs_move_get_result_process(void *arg)
{
  int i = 0, ret = 0;
  int chn_num = (int)arg;
  IMP_IVS_MoveOutput *result = NULL;
  printf("get_result_process komenco\n");
  bool inMovo=false;

  for (i = 0;  ; i++)
  {
    ret = IMP_IVS_PollingResult(chn_num, 1000); // timeout 1s
    if (ret < 0) {
      IMP_LOG_ERR(TAG, "IMP_IVS_PollingResult(%d, %d) failed\n", chn_num, 2000);
      printf("timeout IMP_IVS_PollingResult\n");
      usleep(100000);
      continue;
    }
    ret = IMP_IVS_GetResult(chn_num, (void **)&result);
    if (ret < 0) {
      fprintf(stderr , "%s: IMP_IVS_GetResult(%d) failed\n",TAG, chn_num);
      usleep(100000);
      continue;
    }
    int hasMove=0;
    for( int i=0 ; i< MovoParam.roiRectCnt ; i++)
    {
      if(result->retRoi[i])
      {
        if(!hasMove) printf("Movo detekta en : ");
        hasMove += result->retRoi[i];
        printf( "%d ", i);
      }
    }
    if(hasMove)
    {
      printf(".\n");
      if(!inMovo)
      {
        printf("Movado komenca.\n");
        inMovo=true;
        exec_command(detectionScriptOn, NULL);
      }
      usleep(500000);
    }
    else
    {
      if(inMovo)
      {
        printf("Movado finita.\n");
        inMovo=false;
        exec_command(detectionScriptOff, NULL);
      }
    }

    ret = IMP_IVS_ReleaseResult(chn_num, (void *)result);
    if (ret < 0) {
      IMP_LOG_ERR(TAG, "IMP_IVS_ReleaseResult(%d) failed\n", chn_num);
      return (void *)-1;
    }
    usleep(100000);
  }
  printf("get_result_process fino.\n");

  return (void *)0;
}

static pthread_t ivs_tid;
static pthread_t kopio_tid;

int imp_init()
{
  // Paŝo 1 : inicialigo de la sensilo
  doIMP( IMP_ISP_Open(), "failed to open ISP\n");
  doIMP( IMP_ISP_AddSensor(&sensor_info) , "failed to AddSensor\n");
  doIMP( IMP_ISP_EnableSensor() , "failed to EnableSensor\n");
  IMP_LOG_DBG(TAG, "Paŝo 1 sukceso\n");

  // Paŝo 2 : inicialigo de la IMP-sistemo
  doIMP( IMP_Encoder_SetPoolSize(1036800) , "failed to SetPoolSize\n");
  doIMP( IMP_OSD_SetPoolSize(1048576) , "failed to SetPoolSize\n");
  doIMP( IMP_System_Init() , "IMP_System_Init failed\n");
  /* enable tuning, to tune graphics */
  doIMP( IMP_ISP_EnableTuning() , "IMP_ISP_EnableTuning failed\n");
  IMP_LOG_DBG(TAG, "Paŝo 2 sukceso\n");

  // Pasô 3 : inicialigo de la kadrofontoj
  for (int i = 0; i <= 2  ; i++)
  {
    doIMParg( IMP_FrameSource_CreateChn(i, &fs_chn_attrs[i]) , "IMP_FrameSource_CreateChn(chn%d) error !\n", i);
    doIMParg( IMP_FrameSource_SetChnAttr(i, &fs_chn_attrs[i]) , "IMP_FrameSource_SetChnAttr(chn%d) error !\n",  i);
  }
  IMP_LOG_DBG(TAG, "Paŝo 3 sukceso\n");

  //
  doIMP( IMP_ISP_Tuning_SetISPBypass(IMPISP_TUNING_OPS_MODE_ENABLE) , "IMP_ISP_Tuning_SetISPBypass failed.\n");

  // Paŝo 4
  for (int i = 0; i <= 2 ; i++)
  {
    doIMParg( IMP_Encoder_CreateGroup(i) , "IMP_Encoder_CreateGroup(%d) error !\n", i);
  }
  IMP_LOG_DBG(TAG, "Paŝo 4 sukceso\n");

  // Paŝo 5
  for (int i = 0; i <= 1; i++)
  {
    doIMParg( IMP_Encoder_SetMaxStreamCnt(i, 5) , "IMP_Encoder_SetMaxStreamCnt(%d) error !\n", i);
    doIMParg( IMP_Encoder_CreateChn(i, &channel_attrs[i]) , "IMP_Encoder_CreateChn(%d) error !\n", i);
    doIMP2arg( IMP_Encoder_RegisterChn(i, i) , "IMP_Encoder_RegisterChn(%d, ) error: %d\n" , i, ret);
  }
  IMP_LOG_DBG(TAG, "Paŝo 5 sukceso\n");
  
  // ivs init
  doIMP( IMP_IVS_CreateGroup(0) , "IMP_IVS_CreateGroup(0) failed\n");
  // Bind framesource channel.2-output.0 to IVS group
  IMPCell fs_cell = {DEV_ID_FS, 2, 1};// use FrameSource 2
  IMPCell ivs_cell = {DEV_ID_IVS, 0, 0};
  doIMP( IMP_System_Bind (&fs_cell, &ivs_cell) , "Bind FrameSource IVS\n" );
  IMP_LOG_DBG(TAG, "ivs init sukceso\n");

  // ivs move start
  printf("ivs move start\n");

  IMPIVSInterface *interface=NULL;
  interface = IMP_IVS_CreateMoveInterface(&MovoParam);
  if (interface == NULL)
    IMP_LOG_ERR(TAG, "IMP_IVS_CreateGroup(%d) failed\n", 0);
  doIMP( IMP_IVS_CreateChn(0, interface) , "IMP_IVS_CreateChn failed\n");
  doIMP( IMP_IVS_RegisterChn(0, 0) , "IMP_IVS_RegisterChn failed\n");
  doIMP( IMP_IVS_StartRecvPic(0) , "IMP_IVS_StartRecvPic failed\n");
  // end ivs move start

  // start get result thread
  int chn_num=0;
  printf("ivs thread start\n");
  if (pthread_create(&ivs_tid, NULL, ivs_move_get_result_process, (void *)chn_num) < 0)
    IMP_LOG_ERR(TAG, "create ivs_move_get_result_process failed\n");



  // Paŝo 6
  for (int i = 0; i <= 2 ; i++)
    doIMParg( IMP_System_Bind(&inCells[i], &outCells[i]) , "Bind FrameSource channel%d and Encoder failed\n",i);
  IMP_LOG_DBG(TAG, "Paŝo 6 sukceso\n");

  // Paŝo 7
  for (int i = 0; i <=2 ; i++)
    doIMParg( IMP_FrameSource_EnableChn(i) , "IMP_FrameSource_EnableChn(%d) error\n", i);
  IMP_LOG_DBG(TAG, "Paŝo 7 sukceso\n");

  // Paŝo 8
  for (int i = 0; i <=1 ; i++)
    doIMParg( IMP_Encoder_StartRecvPic(i) , "IMP_Encoder_StartRecvPic(%d) failed\n", i);
  IMP_LOG_DBG(TAG, "Paŝo 7 sukceso\n");
  if (pthread_create(&kopio_tid, NULL, ekrankopio_process, (void *)chn_num) < 0)
    IMP_LOG_ERR(TAG, "create ekrankopio_process failed\n");

  return 0;
}

int imp_exit()
{
  // Paŝo 1
  for (int i = 0; i <=1 ; i++)
    doIMParg( IMP_Encoder_StopRecvPic(i) , "IMP_Encoder_StartRecvPic(%d) failed\n", i);
  for (int i = 0; i <= 1 ; i++)
    doIMP2arg( IMP_FrameSource_DisableChn(i) , "IMP_FrameSource_DisableChn(%d) error: %d\n", ret, i);

  // Paŝo 2
  for (int i = 0; i <= 1 ; i++)
  {
    doIMParg( IMP_System_UnBind(&inCells[i], &outCells[i]) , "UnBind FrameSource channel%d and Encoder failed\n",i);
  }
  
  // ivs exit 
  doIMP( IMP_IVS_DestroyGroup(0), "IMP_IVS_DestroyGroup(0) failed\n");

  // Paŝo 3 : Encoder exit
  for (int encChn = 0; encChn <= 1 ; encChn++)
  {
    IMPEncoderCHNStat chn_stat;
    doIMParg( IMP_Encoder_Query(encChn, &chn_stat) , "IMP_Encoder_Query(%d) error.\n",encChn);

    if (chn_stat.registered)
    {
      doIMParg( IMP_Encoder_UnRegisterChn(encChn) , "IMP_Encoder_UnRegisterChn(%d) error.\n",encChn);
      doIMParg( IMP_Encoder_DestroyChn(encChn) , "IMP_Encoder_DestroyChn(%d) error.\n",encChn);
    }
  }
  for (int i = 0; i <= 1 ; i++)
  {
    doIMParg( IMP_Encoder_DestroyGroup(i) , "IMP_Encoder_DestroyGroup(i) error: %d\n", ret);
  }

  /* Paŝo 4 :  FrameSource exit */
  for (int i = 0; i <= 2 ; i++)
  {
    doIMParg( IMP_FrameSource_DestroyChn(i) , "IMP_FrameSource_DestroyChn() error: %d\n", ret);
  }

  /* Paŝo 5 : System exit */
  IMP_System_Exit();
  doIMP( IMP_ISP_DisableSensor() , "failed to DisableSensor\n");
  doIMP( IMP_ISP_DelSensor(&sensor_info) , "failed to DelSensor\n");
  doIMP( IMP_ISP_DisableTuning() , "IMP_ISP_DisableTuning failed\n");
  doIMP( IMP_ISP_Close() , "failed to close ISP\n");

  return 0;
}


