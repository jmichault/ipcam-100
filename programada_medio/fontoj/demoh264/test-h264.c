
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

#define SENSOR_CUBS_TYPE        TX_SENSOR_CONTROL_INTERFACE_I2C
#define TAG "testimp"

#define doIMP( x , msg ) { int ret= x ; if(ret<0){ IMP_LOG_ERR(TAG, msg);return -1;}}

#define STREAM_FILE_PATH_PREFIX         "/tmp"
#define NR_FRAMES_TO_SAVE   100

#define SENSOR_FRAME_RATE_NUM   20
#define SENSOR_FRAME_RATE_DEN   1
#define SENSOR_NAME       "gc2053"
#define SENSOR_CUBS_TYPE        TX_SENSOR_CONTROL_INTERFACE_I2C
#define SENSOR_I2C_ADDR     0x37
#define SENSOR_WIDTH      1920
#define SENSOR_HEIGHT     1080
#define CHN0_EN                 1
#define CHN1_EN                 1
#define CROP_EN         0

#define SENSOR_WIDTH_SECOND   640
#define SENSOR_HEIGHT_SECOND    480
#define CH0_INDEX  0
#define CH1_INDEX  1

#define ENC_H264_CHANNEL                1
#define ENC_JPEG_CHANNEL                0

#define FS_CHN_NUM      1        //MIN 1,MAX 2

int IMP_Encoder_SetPoolSize(int);
int IMP_OSD_SetPoolSize(int);

IMPSensorInfo sensor_info[2]
={ {.name = SENSOR_NAME,
    .cbus_type = SENSOR_CUBS_TYPE,
    .i2c.type = SENSOR_NAME,
    .i2c.addr = SENSOR_I2C_ADDR
   }
  ,{.name=""}
 };

// 3 framesources :
// canal 0 = haute résolution
// canal 1 = basse résolution
// canal 2 = trés basse résolution, pour détection de mouvement ?
static IMPFSChnAttr fs_chn_attrs[3]=
{
  {
   .picWidth = 1280 , .picHeight = 720 ,
   .pixFmt = PIX_FMT_NV12  ,
   .crop = { .enable = 0 , .left = 0 , .top = 0 , .width = 1920 , .height = 1080 } ,
   .scaler = { .enable = 1 , .outwidth = 1280 , .outheight = 720 } ,
   .outFrmRateNum = 25 , .outFrmRateDen = 1 ,
   .nrVBs = 2 ,
   .type = FS_PHY_CHANNEL ,
   .x = 1 ,
  },
  {
   .picWidth = 640 , .picHeight = 480 ,
   .pixFmt = PIX_FMT_NV12  ,
   .crop = { .enable = 0 , .left = 0 , .top = 0 , .width = 1920 , .height = 1080 } ,
   .scaler = { .enable = 1 , .outwidth = 640 , .outheight = 480 } ,
   .outFrmRateNum = 25 , .outFrmRateDen = 1 ,
   .nrVBs = 2 ,
   .type = FS_PHY_CHANNEL ,
   .x = 1 ,
  },
  {
   .picWidth = 320 , .picHeight = 184 ,
   .pixFmt = PIX_FMT_NV12  ,
   .crop = { .enable = 0 , .left = 0 , .top = 0 , .width = 1920 , .height = 1080 } ,
   .scaler = { .enable = 1 , .outwidth = 320 , .outheight = 184 } ,
   .outFrmRateNum = 5 , .outFrmRateDen = 1 ,
   .nrVBs = 2 ,
   .type = FS_PHY_CHANNEL ,
   .x = 0 ,
  },
};

// 2 canaux encodeur :
// 0 = haute résolution
// 1 = basse résolution
IMPEncoderCHNAttr channel_attrs[2] =
{
  {
    .encAttr =
      {
        .enType=PT_H264 , .bufSize=0 , .profile=2 , //
        .picWidth=1280, .picHeight=720 ,
        .crop = { .enable = 0 , .x = 0 , .y = 0 , .w = 0 , .h = 0 } ,
        .userData = { .maxUserDataCnt = 2 , .maxUserDataSize = 128 } ,
      },
    .rcAttr =
      {
        .outFrmRate = { .frmRateNum=12 , .frmRateDen=1 },
        .maxGop = 24,
        .attrRcMode = {
          .rcMode = ENC_RC_MODE_SMART , // 3
          .attrH264Vbr= {
            .maxQp=48 , .minQp=15 ,
            .staticTime = 2 ,
            .maxBitRate = 600 ,
            .iBiasLvl = 0,
            .changePos = 80,
            .qualityLvl = 2,
            .frmQPStep = 3, .gopQPStep = 15,
            .gopRelation = 0,
           },
         },
        .attrFrmUsed = {
            .enable=0 , .frmUsedMode=ENC_FRM_BYPASS , .frmUsedTimes=0,
         },
        .attrDenoise = {
            .enable =0 , .dnType=0 , .dnIQp = 23 , .dnPQp=1 ,
         },
        .attrHSkip = {
            .hSkipAttr = {.skipType=3 , .m=0 , .n=0 ,.maxSameSceneCnt=0 ,.bEnableScenecut=0 ,.bBlackEnhance= 0},
            .maxHSkipType = 0,
         },
      },
    .xW=1280 , .xH = 720,
  },
  {
    .encAttr =
      {
        .enType=PT_H264 , .bufSize=0 , .profile=2 ,
        .picWidth=640 , .picHeight=480 ,
        .crop = { .enable = 0 , .x = 0 , .y = 0 , .w = 0 , .h = 0 } ,
        .userData = { .maxUserDataCnt = 2 , .maxUserDataSize = 128 } ,
      },
    .rcAttr =
      {
        .outFrmRate = { .frmRateNum=10 , .frmRateDen=1 },
        .maxGop = 20,
        .attrRcMode = {
          .rcMode = ENC_RC_MODE_VBR , // 2 ENC_RC_MODE_VBR
          .attrH264Vbr= {
            .maxQp=48 , .minQp=20 ,
            .staticTime = 2 ,
            .maxBitRate = 512 ,
            .iBiasLvl = 0,
            .changePos = 80,
            .qualityLvl = 2,
            .frmQPStep = 3, .gopQPStep = 15,
            .gopRelation = 0,
           },
         },
        .attrFrmUsed = {
            .enable=0 , .frmUsedMode=ENC_FRM_BYPASS , .frmUsedTimes=0,
         },
        .attrDenoise = {
            .enable =0 , .dnType=0 , .dnIQp = 0 , .dnPQp=0 ,
         },
        .attrHSkip = {
            .hSkipAttr = {.skipType=0 , .m=0 , .n=0 ,.maxSameSceneCnt=0 ,.bEnableScenecut=0 ,.bBlackEnhance= 0},
            .maxHSkipType = 0,
         },
      },
    .xW=720 , .xH = 576,
  },
};


// cellules :
IMPCell inCells[] = 
{
 { .deviceID=DEV_ID_FS, .groupID=0, .outputID=0},
 { .deviceID=DEV_ID_FS, .groupID=1, .outputID=0},
};
IMPCell outCells[] = 
{
 { .deviceID=DEV_ID_ENC, .groupID=0, .outputID=0},
 { .deviceID=DEV_ID_ENC, .groupID=1, .outputID=0},
};


// définie plus loin
void *get_h264_stream(void *args);

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
    //IMP_LOG_DBG(TAG, "stream->pack[%d].dataType=%d\n", i, ((int)stream->pack[i].dataType.h264Type));
  }

  return 0;
}


static int encoder_chn_exit(int encChn)
{
  int ret;
  IMPEncoderCHNStat chn_stat;
  ret = IMP_Encoder_Query(encChn, &chn_stat);
  if (ret < 0)
  {
    IMP_LOG_ERR(TAG, "IMP_Encoder_Query(%d) error: %d\n",
      encChn, ret);
    return -1;
  }

  if (chn_stat.registered)
  {
    ret = IMP_Encoder_UnRegisterChn(encChn);
    if (ret < 0)
    {
      IMP_LOG_ERR(TAG, "IMP_Encoder_UnRegisterChn(%d) error: %d\n",
        encChn, ret);
      return -1;
    }

    ret = IMP_Encoder_DestroyChn(encChn);
    if (ret < 0)
    {
      IMP_LOG_ERR(TAG, "IMP_Encoder_DestroyChn(%d) error: %d\n",
        encChn, ret);
      return -1;
    }
  }

  return 0;
}


int main(int argc, char **argv)
{
  int ret = 0;
  //step 1 system_init
  IMP_LOG_DBG(TAG, "sample_system_init start\n");

  doIMP( IMP_ISP_Open(), "failed to open ISP\n");
  doIMP( IMP_ISP_AddSensor(&sensor_info[0]) , "failed to AddSensor\n");
  doIMP( IMP_ISP_EnableSensor() , "failed to EnableSensor\n");
  doIMP( IMP_Encoder_SetPoolSize(1036800) , "failed to SetPoolSize\n");
  doIMP( IMP_OSD_SetPoolSize(1048576) , "failed to SetPoolSize\n");

  doIMP( IMP_System_Init() , "IMP_System_Init failed\n");
  /* enable tuning, to tune graphics */
  doIMP( IMP_ISP_EnableTuning() , "IMP_ISP_EnableTuning failed\n");
  doIMP( IMP_ISP_Tuning_SetSaturation(128) , "IMP_ISP_Tuning_SetSaturation failed\n");
  doIMP( IMP_ISP_Tuning_SetBrightness(128) , "IMP_ISP_Tuning_SetBrightness failed\n");
  IMPVersion pstVersion;
  memset(&pstVersion,0,sizeof(pstVersion));

  ret = IMP_System_GetVersion(&pstVersion);
  if(ret != 0)
    IMP_LOG_ERR(TAG, "IMP_System_GetVersion failed\n");
  else
    IMP_LOG_DBG(TAG, "IMP version :%s\n", pstVersion.aVersion);

  const char * cpu = IMP_System_GetCPUInfo();
  IMP_LOG_DBG(TAG, "CPU :%s\n", cpu);
  IMP_LOG_DBG(TAG, "ImpSystemInit success\n");
  // step2 : sample_framesource_init

  for (int i = 0; i <= 2  ; i++)
  {
      ret = IMP_FrameSource_CreateChn(i, &fs_chn_attrs[i]);
      if(ret < 0)
      {
        IMP_LOG_ERR(TAG, "IMP_FrameSource_CreateChn(chn%d) error !\n", i);
        return -1;
      }

      ret = IMP_FrameSource_SetChnAttr(i, &fs_chn_attrs[i]);
      if (ret < 0)
      {
        IMP_LOG_ERR(TAG, "IMP_FrameSource_SetChnAttr(chn%d) error !\n",  i);
        return -1;
      }
      IMP_LOG_DBG(TAG, "1.1 %d success\n",i);
  }
  IMP_LOG_DBG(TAG, "2.1 success\n");
  ret = IMP_ISP_Tuning_SetSensorFPS(20,1);
  if (ret < 0)
  {
    IMP_LOG_ERR(TAG, "IMP_ISP_Tuning_SetSensorFPS failed.\n");
    return -1;
  }
  ret = IMP_ISP_Tuning_SetISPBypass(IMPISP_TUNING_OPS_MODE_ENABLE);
  if (ret < 0)
  {
    IMP_LOG_ERR(TAG, "IMP_ISP_Tuning_SetISPBypass failed.\n");
    return -1;
  }

  // step 2.2 :
  for (int i = 0; i <= 1 ; i++)
  {
    {
      ret = IMP_Encoder_CreateGroup(i);
      if (ret < 0)
      {
        IMP_LOG_ERR(TAG, "IMP_Encoder_CreateGroup(%d) error !\n", i);
        return -1;
      }
      IMP_LOG_DBG(TAG, "2.2 %d success\n",i);
    }
  }
  IMP_LOG_DBG(TAG, "2.2 success\n");

  // Step.3 Encoder init  sample_encoder_init();

  for (int i = 0; i <= 1; i++)
  {
      ret = IMP_Encoder_SetMaxStreamCnt(i, 5);
      if (ret < 0)
      {
        IMP_LOG_ERR(TAG, "IMP_Encoder_SetMaxStreamCnt(%d) error !\n", i);
        return -1;
      }
      ret = IMP_Encoder_CreateChn(i, &channel_attrs[i]);
      if (ret < 0)
      {
        IMP_LOG_ERR(TAG, "IMP_Encoder_CreateChn(%d) error !\n", i);
        return -1;
      }

      ret = IMP_Encoder_RegisterChn(i, i);
      if (ret < 0)
      {
        IMP_LOG_ERR(TAG, "IMP_Encoder_RegisterChn(%d, %d) error: %d\n",
          i,i, ret);
        return -1;
      }
      IMP_LOG_DBG(TAG, "3.1 %d success\n",i);
  }
  IMP_LOG_DBG(TAG, "3 success\n");

  /* Step.4 Bind */
  for (int i = 0; i <= 1 ; i++)
  {
    {
      ret = IMP_System_Bind(&inCells[i], &outCells[i]);
      if (ret < 0)
      {
        IMP_LOG_ERR(TAG, "Bind FrameSource channel%d and Encoder failed\n",i);
        return -1;
      }
      IMP_LOG_DBG(TAG, "4.1 %d success\n",i);
    }
  }
  IMP_LOG_DBG(TAG, "4 success\n");

  /* Step.5 Stream On */
  /* Enable channels */
  for (int i = 0; i < FS_CHN_NUM; i++)
  {
    {
      ret = IMP_FrameSource_EnableChn(i);
      if (ret < 0)
      {
        IMP_LOG_ERR(TAG, "IMP_FrameSource_EnableChn(%d) error: %d\n", ret, i);
        return -1;
      }
      IMP_LOG_DBG(TAG, "5.1 %d success\n",i);
    }
  }
  IMP_LOG_DBG(TAG, "5 success\n");

  /* Step.6 Get stream */
  pthread_t tid[FS_CHN_NUM];
  int pthread_ids[2]={0,1};

  for (int i = 0; i < FS_CHN_NUM; i++)
  {
    {
      ret = pthread_create(&tid[i], NULL, get_h264_stream, &pthread_ids[i]);
      if (ret < 0)
      {
        IMP_LOG_ERR(TAG, "Create Chn%d get_h264_stream \n",i);
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

  /* Exit sequence as follow */

  /* Step.a Stream Off */
  for (int i = 0; i < FS_CHN_NUM; i++)
  {
    {
      ret = IMP_FrameSource_DisableChn(i);
      if (ret < 0)
      {
        IMP_LOG_ERR(TAG, "IMP_FrameSource_DisableChn(%d) error: %d\n", ret, i);
        return -1;
      }
    }
  }

  /* Step.b UnBind */
  for (int i = 0; i <= 1 ; i++)
  {
    {
      ret = IMP_System_UnBind(&inCells[i], &outCells[i]);
      if (ret < 0)
      {
        IMP_LOG_ERR(TAG, "UnBind FrameSource channel%d and Encoder failed\n",i);
        return -1;
      }
    }
  }

  /* Step.c Encoder exit */
  ret = encoder_chn_exit(ENC_H264_CHANNEL);
  if (ret < 0)
  {
    IMP_LOG_ERR(TAG, "Encoder Channel %d exit  error: %d\n",
      ENC_H264_CHANNEL, ret);
  }

  ret = encoder_chn_exit(ENC_JPEG_CHANNEL);
  if (ret < 0)
  {
    IMP_LOG_ERR(TAG, "Encoder Channel %d exit  error: %d\n",
      ENC_JPEG_CHANNEL, ret);
  }

  ret = IMP_Encoder_DestroyGroup(0);
  if (ret < 0)
  {
    IMP_LOG_ERR(TAG, "IMP_Encoder_DestroyGroup(0) error: %d\n", ret);
  }

  /* Step.d FrameSource exit */
  for (int i = 0; i <  FS_CHN_NUM; i++)
  {
    {
      /*Destroy channel i*/
      ret = IMP_FrameSource_DestroyChn(i);
      if (ret < 0)
      {
        IMP_LOG_ERR(TAG, "IMP_FrameSource_DestroyChn() error: %d\n", ret);
        return -1;
      }
    }
  }

  /* Step.e System exit */
  IMP_System_Exit();

  ret = IMP_ISP_DisableSensor();
  if(ret < 0)
  {
    IMP_LOG_ERR(TAG, "failed to EnableSensor\n");
    return -1;
  }

  ret = IMP_ISP_DelSensor(&sensor_info[0]);
  if(ret < 0)
  {
    IMP_LOG_ERR(TAG, "failed to AddSensor\n");
    return -1;
  }

  ret = IMP_ISP_DisableTuning();
  if(ret < 0)
  {
    IMP_LOG_ERR(TAG, "IMP_ISP_DisableTuning failed\n");
    return -1;
  }

  if(IMP_ISP_Close())
  {
    IMP_LOG_ERR(TAG, "failed to open ISP\n");
    return -1;
  }

  IMP_LOG_DBG(TAG, " sample_system_exit success\n");

  return 0;
}


void *get_h264_stream(void *args)
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

  sprintf(stream_path, "%s/stream-%d.h264",
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
    /* Get H264 Stream */
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
