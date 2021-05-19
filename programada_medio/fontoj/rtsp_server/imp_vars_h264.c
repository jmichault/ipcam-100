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

// sensilotrajtoj
#define SENSOR_FRAME_RATE_NUM   20
#define SENSOR_FRAME_RATE_DEN   1
#define SENSOR_NAME       "gc2053"
#define SENSOR_CUBS_TYPE        TX_SENSOR_CONTROL_INTERFACE_I2C
#define SENSOR_I2C_ADDR     0x37
#define SENSOR_WIDTH      1920
#define SENSOR_HEIGHT     1080

// apriora dimensio de la unua rivereto
#define ELIGO_WIDTH   1920
#define ELIGO_HEIGHT  1080

// apriora dimensio de la dua rivereto
#define ELIGO_WIDTH_DUA   640
#define ELIGO_HEIGHT_DUA  360



IMPSensorInfo sensor_info
={ .name = SENSOR_NAME,
   .cbus_type = SENSOR_CUBS_TYPE,
   .i2c.type = SENSOR_NAME,
   .i2c.addr = SENSOR_I2C_ADDR
 };

// 3 framesources :
// canal 0 = haute résolution
// canal 1 = basse résolution
// canal 2 = trés basse résolution, pour détection de mouvement ?
IMPFSChnAttr fs_chn_attrs[3]=
{
  {
   .picWidth = ELIGO_WIDTH , .picHeight = ELIGO_HEIGHT ,
   .pixFmt = PIX_FMT_NV12  ,
   .crop = { .enable = 0 , .left = 0 , .top = 0 , .width = SENSOR_WIDTH , .height = SENSOR_HEIGHT } ,
   .scaler = { .enable = 0 , .outwidth = ELIGO_WIDTH , .outheight = ELIGO_HEIGHT } ,
   .outFrmRateNum = 25 , .outFrmRateDen = 1 ,
   .nrVBs = 2 ,
   .type = FS_PHY_CHANNEL ,
   .x = 1 ,
  },
  {
   .picWidth = ELIGO_WIDTH_DUA , .picHeight = ELIGO_HEIGHT_DUA ,
   .pixFmt = PIX_FMT_NV12  ,
   .crop = { .enable = 0 , .left = 0 , .top = 0 , .width = SENSOR_WIDTH , .height = SENSOR_HEIGHT } ,
   .scaler = { .enable = 1 , .outwidth = ELIGO_WIDTH_DUA , .outheight = ELIGO_HEIGHT_DUA } ,
   .outFrmRateNum = 25 , .outFrmRateDen = 1 ,
   .nrVBs = 2 ,
   .type = FS_PHY_CHANNEL ,
   .x = 1 ,
  },
  {
   .picWidth = 320 , .picHeight = 184 ,
   .pixFmt = PIX_FMT_NV12  ,
   .crop = { .enable = 0 , .left = 0 , .top = 0 , .width = SENSOR_WIDTH , .height = SENSOR_HEIGHT } ,
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
        .picWidth=ELIGO_WIDTH, .picHeight=ELIGO_HEIGHT ,
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
    .xW=ELIGO_WIDTH , .xH = ELIGO_HEIGHT,
  },
  {
    .encAttr =
      {
        .enType=PT_H264 , .bufSize=0 , .profile=2 ,
        .picWidth=ELIGO_WIDTH_DUA , .picHeight=ELIGO_HEIGHT_DUA ,
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
    .xW=ELIGO_WIDTH_DUA , .xH = ELIGO_HEIGHT_DUA,
  },
  {
    .encAttr =
      {
        .enType=PT_JPEG , .bufSize=0 , .profile=0 ,
        .picWidth=320 , .picHeight=184 ,
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
    .xW=320 , .xH = 184,
  },
};


// cellules :
IMPCell inCells[] = 
{
 { .deviceID=DEV_ID_FS, .groupID=0, .outputID=0},
 { .deviceID=DEV_ID_FS, .groupID=1, .outputID=0},
 { .deviceID=DEV_ID_FS, .groupID=2, .outputID=0},
};
IMPCell outCells[] = 
{
 { .deviceID=DEV_ID_ENC, .groupID=0, .outputID=0},
 { .deviceID=DEV_ID_ENC, .groupID=1, .outputID=0},
 { .deviceID=DEV_ID_ENC, .groupID=2, .outputID=0},
};

