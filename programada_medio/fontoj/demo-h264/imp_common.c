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

#define TAG "imp_common"

int imp_init()
{
  // Paŝo 1 : inicialigo de la sensilo
  doIMP( IMP_ISP_Open(), "failed to open ISP\n");
  doIMP( IMP_ISP_AddSensor(&sensor_info[0]) , "failed to AddSensor\n");
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
  for (int i = 0; i <= 1 ; i++)
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

  // Paŝo 6
  for (int i = 0; i <= 1 ; i++)
  {
    doIMParg( IMP_System_Bind(&inCells[i], &outCells[i]) , "Bind FrameSource channel%d and Encoder failed\n",i);
  }
  IMP_LOG_DBG(TAG, "Paŝo 6 sukceso\n");

  // Paŝo 7
  for (int i = 0; i <=1 ; i++)
  {
    doIMParg( IMP_FrameSource_EnableChn(i) , "IMP_FrameSource_EnableChn(%d) error\n", i);
  }
  IMP_LOG_DBG(TAG, "Paŝo 7 sukceso\n");
  return 0;
}

int imp_exit()
{
  // Paŝo 1
  for (int i = 0; i <= 1 ; i++)
  {
    doIMP2arg( IMP_FrameSource_DisableChn(i) , "IMP_FrameSource_DisableChn(%d) error: %d\n", ret, i);
  }

  // Paŝo 2
  for (int i = 0; i <= 1 ; i++)
  {
    doIMParg( IMP_System_UnBind(&inCells[i], &outCells[i]) , "UnBind FrameSource channel%d and Encoder failed\n",i);
  }

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
  doIMP( IMP_ISP_DelSensor(&sensor_info[0]) , "failed to DelSensor\n");
  doIMP( IMP_ISP_DisableTuning() , "IMP_ISP_DisableTuning failed\n");
  doIMP( IMP_ISP_Close() , "failed to close ISP\n");

  return 0;
}


