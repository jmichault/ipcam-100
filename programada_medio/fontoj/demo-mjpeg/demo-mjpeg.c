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
#include <imp/imp_common.h>
#include <imp/imp_framesource.h>
#include <imp/imp_isp.h>
#include <imp/imp_system.h>
#include <imp/imp_framesource.h>
#include <imp/imp_encoder.h>
#include <imp/imp_ivs.h>
#include <imp/imp_ivs_move.h>
#include <imp/imp_osd.h>

#include "imp_common.h"


#define STREAM_FILE_PATH_PREFIX         "/tmp"
#define NR_FRAMES_TO_SAVE   100
#define FS_CHN_NUM      2        //MIN 1,MAX 2

#define TAG "demo-mjpeg"

// définie plus loin
void *get_mjpeg_stream(void *args);

static int save_stream(int fd, IMPEncoderStream *stream)
{
  int ret, i, nr_pack = stream->packCount;

  for (i = 0; i < nr_pack; i++)
  {
    ret = write(fd, (void *)stream->pack[i].virAddr,
      stream->pack[i].length);
    if (ret != stream->pack[i].length)
    {
      IMP_LOG_ERR(TAG, "stream write error:%s\n", strerror(errno));
      return -1;
    }
  }

  return 0;
}

int main(int argc, char **argv)
{
 int ret = 0;
  // ŝanĝas agordojn de kodigilo al mjpg
  for ( int i = 0; i < 1; i++)
  {
    channel_attrs[i].encAttr.enType=PT_JPEG;
    channel_attrs[i].encAttr.profile=0;
  }

  // system_init
  IMP_LOG_DBG(TAG, "system_init start\n");
  imp_init();

  // ŝanĝo de la satureco kaj la brileco
  doIMP( IMP_ISP_Tuning_SetSaturation(150) , "IMP_ISP_Tuning_SetSaturation failed\n");
  doIMP( IMP_ISP_Tuning_SetBrightness(180) , "IMP_ISP_Tuning_SetBrightness failed\n");

  // legi la versiojn
  IMPVersion pstVersion;
  memset(&pstVersion,0,sizeof(pstVersion));
  ret = IMP_System_GetVersion(&pstVersion);
  if(ret != 0)
    IMP_LOG_ERR(TAG, "IMP_System_GetVersion failed\n");
  else
    IMP_LOG_DBG(TAG, "IMP version :%s\n", pstVersion.aVersion);
  const char * cpu = IMP_System_GetCPUInfo();
  IMP_LOG_DBG(TAG, "CPU :%s\n", cpu);

  // ŝanĝo de FPS
  doIMP( IMP_ISP_Tuning_SetSensorFPS(20,1) , "IMP_ISP_Tuning_SetSensorFPS failed.\n");


  /* Get stream */
  pthread_t tid[FS_CHN_NUM];
  int pthread_ids[2]={0,1};

  for (int i = 0; i < FS_CHN_NUM; i++)
  {
    {
      ret = pthread_create(&tid[i], NULL, get_mjpeg_stream, &pthread_ids[i]);
      if (ret < 0)
      {
        IMP_LOG_ERR(TAG, "Create Chn%d get_mjpeg_stream \n",i);
      }
      else
        IMP_LOG_DBG(TAG, "6.1 %d success\n",i);
    }
  }
  IMP_LOG_DBG(TAG, "6.1 success\n");

  for (int i = 0; i < FS_CHN_NUM; i++)
  {
    {
      pthread_join(tid[i],NULL);
    }
  }
  IMP_LOG_DBG(TAG, "6.2 success\n");

  imp_exit();
  IMP_LOG_DBG(TAG, " sample_system_exit success\n");
  printf("Fino !\n");  

  return 0;
}


void *get_mjpeg_stream(void *args)
{
  int i, j, ret;
  char stream_path[64];

  i = (int ) (*((int*)args));
  IMP_LOG_DBG(TAG, "Stream %d ", i);

  ret = IMP_Encoder_StartRecvPic(i);
  if (ret < 0)
  {
    IMP_LOG_ERR(TAG, "IMP_Encoder_StartRecvPic(%d) failed\n", i);
    return ((void *)-1);
  }

  sprintf(stream_path, "%s/stream-%d.mjpeg",
    STREAM_FILE_PATH_PREFIX, i);

  IMP_LOG_DBG(TAG, "Open Stream file %s ", stream_path);
  int stream_fd = open(stream_path, O_RDWR | O_CREAT | O_TRUNC, 0777);
  if (stream_fd < 0)
  {
    IMP_LOG_ERR(TAG, "failed: %s\n", strerror(errno));
    return ((void *)-1);
  }
  IMP_LOG_DBG(TAG, "OK\n");

  for (j = 0; j < NR_FRAMES_TO_SAVE; j++)
  {
    ret = IMP_Encoder_PollingStream(i, 1000);
    if (ret < 0)
    {
      IMP_LOG_ERR(TAG, "Polling stream timeout, j=%d\n",j);
      continue;
    }

    IMPEncoderStream stream;
    /* Get mjpeg Stream */
    ret = IMP_Encoder_GetStream(i, &stream, 1);
    if (ret < 0)
    {
      IMP_LOG_ERR(TAG, "IMP_Encoder_GetStream() failed\n");
      return ((void *)-1);
    }
    printf( "stream :i=%d,j=%d, packCount=%d,seq=%d, refType=%d\n", i,j, stream.packCount,stream.seq, stream.refType);

    ret = save_stream(stream_fd, &stream);
    if (ret < 0)
    {
      close(stream_fd);
      return ((void *)ret);
    }

    IMP_Encoder_ReleaseStream(i, &stream);
  }

  close(stream_fd);

  ret = IMP_Encoder_StopRecvPic(i);
  if (ret < 0)
  {
    IMP_LOG_ERR(TAG, "IMP_Encoder_StopRecvPic() failed\n");
    return ((void *)-1);
  }

  return ((void *)0);
}
