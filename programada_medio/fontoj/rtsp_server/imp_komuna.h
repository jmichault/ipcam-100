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

#include <imp/imp_common.h>
#include <imp/imp_framesource.h>
#include <imp/imp_isp.h>
#include <imp/imp_system.h>
#include <imp/imp_encoder.h>



#ifdef __cplusplus
extern "C"
{
#endif

/*
 * variabloj uzataj por pravalorizi IMP 
 */
extern IMPSensorInfo sensor_info;
extern IMPFSChnAttr fs_chn_attrs[3];
extern IMPEncoderCHNAttr channel_attrs[3];
extern IMPCell inCells[3]; 
extern IMPCell outCells[3];

/*
 *
 */
int imp_init();

/*
 *
 */
int imp_exit();

#ifdef __cplusplus
}
#endif

/*
 * plej multaj IMP-funkcioj redonas valoron <0 kiam ili malsukcesas, do oni preskaŭ ĉiam vokas ilin farante:
 *
 *   ret = IMP_xxx(...);
 *   if(ret<0)
 *   {
 *     IMP_LOG_ERR(TAG, "......");
 *     return -1;
 *   }
 *
 * Ĉi tiuj tri makrooj ebligas kompakti la kodon kaj fari ĝin pli legebla: 
 */
#define doIMP( x , msg ) { int ret= x ; if(ret<0){ fprintf(stderr,"%s: %s\n",TAG, msg);return -1;}}
#define doIMParg( x , msg , arg ) { int ret= x ; if(ret<0){fprintf(stderr,"%s: ",TAG); fprintf(stderr,msg, arg);return -1;}}
#define doIMP2arg( x , msg , arg1 , arg2 ) { int ret= x ; if(ret<0){fprintf(stderr,"%s: ",TAG); fprintf(stderr, msg, arg1, arg2);return -1;}}

