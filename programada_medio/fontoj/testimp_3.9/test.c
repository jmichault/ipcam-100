
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

#define doIMP( x , msg ) { ret= x ; if(ret<0){ IMP_LOG_ERR(TAG, msg);return -1;}}

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

struct chn_conf
{
  unsigned int index;            //0 for main channel ,1 for second channel
  unsigned int enable;
  IMPFSChnAttr fs_chn_attr;
  IMPCell framesource_chn;
  IMPCell imp_encoder;
};

struct chn_conf chn[] =
{
  {
    .index = CH0_INDEX,
    .enable = CHN0_EN,
    .fs_chn_attr =
    {
      .pixFmt = PIX_FMT_NV12,
      .outFrmRateNum = SENSOR_FRAME_RATE_NUM,
      .outFrmRateDen = SENSOR_FRAME_RATE_DEN,
      .nrVBs = 2,
      .type = FS_PHY_CHANNEL,

      .crop.enable = CROP_EN,
      .crop.top = 0,
      .crop.left = 0,
      .crop.width = SENSOR_WIDTH,
      .crop.height = SENSOR_HEIGHT,

      .scaler.enable = 1,
      .scaler.outwidth = SENSOR_WIDTH_SECOND,
      .scaler.outheight = SENSOR_HEIGHT_SECOND,

      .picWidth = SENSOR_WIDTH_SECOND,
      .picHeight = SENSOR_HEIGHT_SECOND,
    },
    .framesource_chn =  { DEV_ID_FS, 0, 0},
    .imp_encoder = { DEV_ID_ENC, 0, 0},
  },
  {
    .index = CH1_INDEX,
    .enable = CHN1_EN,
    .fs_chn_attr =
    {
      .pixFmt = PIX_FMT_NV12,
      .outFrmRateNum = SENSOR_FRAME_RATE_NUM,
      .outFrmRateDen = SENSOR_FRAME_RATE_DEN,
      .nrVBs = 2,
      .type = FS_PHY_CHANNEL,

      .crop.enable = CROP_EN,
      .crop.top = 0,
      .crop.left = 0,
      .crop.width = SENSOR_WIDTH,
      .crop.height = SENSOR_HEIGHT,

      .scaler.enable = 1,
      .scaler.outwidth = 640,
      .scaler.outheight = 480,

      .picWidth = 640,
      .picHeight = 480,
    },
    .framesource_chn =  { DEV_ID_FS, 1, 0},
    .imp_encoder = { DEV_ID_ENC, 1, 0},
  },
};

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
  doIMP( IMP_Encoder_SetPoolSize(0x200000) , "failed to SetPoolSize\n");
  doIMP( IMP_OSD_SetPoolSize(0x160000) , "failed to SetPoolSize\n");

  doIMP( IMP_System_Init() , "IMP_System_Init failed\n");
  /* enable tuning, to tune graphics */
  doIMP( IMP_ISP_EnableTuning() , "IMP_ISP_EnableTuning failed\n");
  //doIMP( IMP_ISP_Tuning_SetSaturation(128) , "IMP_ISP_Tuning_SetSaturation failed\n");
  //doIMP( IMP_ISP_Tuning_SetBrightness(128) , "IMP_ISP_Tuning_SetBrightness failed\n");
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

  for (int i = 0; i <  FS_CHN_NUM; i++)
  {
    if (chn[i].enable)
    {
      ret = IMP_FrameSource_CreateChn(chn[i].index, &chn[i].fs_chn_attr);
      if(ret < 0)
      {
        IMP_LOG_ERR(TAG, "IMP_FrameSource_CreateChn(chn%d) error !\n", chn[i].index);
        return -1;
      }

      ret = IMP_FrameSource_SetChnAttr(chn[i].index, &chn[i].fs_chn_attr);
      if (ret < 0)
      {
        IMP_LOG_ERR(TAG, "IMP_FrameSource_SetChnAttr(chn%d) error !\n",  chn[i].index);
        return -1;
      }
      IMP_LOG_DBG(TAG, "1.1 %d success\n",i);
    }
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
  for (int i = 0; i < FS_CHN_NUM; i++)
  {
    if (chn[i].enable)
    {
      ret = IMP_Encoder_CreateGroup(chn[i].index);
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
  IMPEncoderAttr *enc_attr;
  IMPEncoderRcAttr *rc_attr;
  IMPFSChnAttr *imp_chn_attr_tmp;
  IMPEncoderCHNAttr channel_attr;

  for (int i = 0; i <  FS_CHN_NUM; i++)
  {
    if (chn[i].enable)
    {
      imp_chn_attr_tmp = &chn[i].fs_chn_attr;
      memset(&channel_attr, 0, sizeof(IMPEncoderCHNAttr));
      enc_attr = &channel_attr.encAttr;
      enc_attr->enType = PT_H264;
      enc_attr->bufSize = 0;
      enc_attr->profile = 1;
      enc_attr->picWidth = imp_chn_attr_tmp->picWidth;
      enc_attr->picHeight = imp_chn_attr_tmp->picHeight;
      rc_attr = &channel_attr.rcAttr;

      rc_attr->rcMode = ENC_RC_MODE_H264CBR;
        rc_attr->rcMode.attrH264Cbr.maxQp = 45;
        rc_attr->rcMode.attrH264Cbr.minQp = 15;
        rc_attr->rcMode.attrH264Cbr.iBiasLvl = 0;
        rc_attr->rcMode.attrH264Cbr.frmQPStep = 3;
        rc_attr->rcMode.attrH264Cbr.gopQPStep = 15;
        rc_attr->rcMode.attrH264Cbr.adaptiveMode = false;
        rc_attr->rcMode.attrH264Cbr.gopRelation = false;

        rc_attr->attrHSkip.hSkipAttr.skipType = (IMPSkipType) skiptype; //IMP_Encoder_STYPE_N1X;
        rc_attr->attrHSkip.hSkipAttr.m = 0;
        rc_attr->attrHSkip.hSkipAttr.n = 0;
        rc_attr->attrHSkip.hSkipAttr.maxSameSceneCnt = maxSameSceneCnt;
        rc_attr->attrHSkip.hSkipAttr.bEnableScenecut = bEnableScenecut;
        rc_attr->attrHSkip.hSkipAttr.bBlackEnhance = 0;
        rc_attr->attrHSkip.maxHSkipType = (IMPSkipType) skiptype; //IMP_Encoder_STYPE_N1X;


      //ret = IMP_Encoder_SetMaxStreamCnt(chn[i].index, 1);
      if (ret < 0)
      {
        IMP_LOG_ERR(TAG, "IMP_Encoder_SetMaxStreamCnt(%d) error !\n", i);
        return -1;
      }
      ret = IMP_Encoder_CreateChn(chn[i].index, &channel_attr);
      if (ret < 0)
      {
        IMP_LOG_ERR(TAG, "IMP_Encoder_CreateChn(%d) error !\n", i);
        return -1;
      }

      ret = IMP_Encoder_RegisterChn(chn[i].index, chn[i].index);
      if (ret < 0)
      {
        IMP_LOG_ERR(TAG, "IMP_Encoder_RegisterChn(%d, %d) error: %d\n",
          chn[i].index, chn[i].index, ret);
        return -1;
      }
      IMP_LOG_DBG(TAG, "3.1 %d success\n",i);
    }
  }
  IMP_LOG_DBG(TAG, "3 success\n");
  /* Step.4 Bind */
  for (int i = 0; i < FS_CHN_NUM; i++)
  {
    if (chn[i].enable)
    {
      ret = IMP_System_Bind(&chn[i].framesource_chn, &chn[i].imp_encoder);
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
    if (chn[i].enable)
    {
      ret = IMP_FrameSource_EnableChn(chn[i].index);
      if (ret < 0)
      {
        IMP_LOG_ERR(TAG, "IMP_FrameSource_EnableChn(%d) error: %d\n", ret, chn[i].index);
        return -1;
      }
      IMP_LOG_DBG(TAG, "5.1 %d success\n",i);
    }
  }
  IMP_LOG_DBG(TAG, "5 success\n");

  /* Step.6 Get stream */
  pthread_t tid[FS_CHN_NUM];

  for (int i = 0; i < FS_CHN_NUM; i++)
  {
    if (chn[i].enable)
    {
      ret = pthread_create(&tid[i], NULL, get_h264_stream, &chn[i].index);
      if (ret < 0)
      {
        IMP_LOG_ERR(TAG, "Create Chn%d get_h264_stream \n",chn[i].index);
      }
      else
        IMP_LOG_DBG(TAG, "6.1 %d success\n",i);
    }
  }
  IMP_LOG_DBG(TAG, "6.1 success\n");

  for (int i = 0; i < FS_CHN_NUM; i++)
  {
    if (chn[i].enable)
    {
      pthread_join(tid[i],NULL);
    }
  }
  IMP_LOG_DBG(TAG, "6.2 success\n");

  /* Exit sequence as follow */

  /* Step.a Stream Off */
  for (int i = 0; i < FS_CHN_NUM; i++)
  {
    if (chn[i].enable)
    {
      ret = IMP_FrameSource_DisableChn(chn[i].index);
      if (ret < 0)
      {
        IMP_LOG_ERR(TAG, "IMP_FrameSource_DisableChn(%d) error: %d\n", ret, chn[i].index);
        return -1;
      }
    }
  }

  /* Step.b UnBind */
  for (int i = 0; i < FS_CHN_NUM; i++)
  {
    if (chn[i].enable)
    {
      ret = IMP_System_UnBind(&chn[i].framesource_chn, &chn[i].imp_encoder);
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
    if (chn[i].enable)
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
    //IMP_LOG_DBG(TAG, "i=%d, stream.packCount=%d, stream.h264RefType=%d\n", i, stream.packCount, stream.h264RefType);

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
