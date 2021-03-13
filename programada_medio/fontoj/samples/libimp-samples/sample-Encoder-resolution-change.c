/*
 * sample-Encoder-resolution-change.c
 *
 * Copyright (C) 2017 Ingenic Semiconductor Co.,Ltd
 */

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>

#include <imp/imp_common.h>
#include <imp/imp_system.h>
#include <imp/imp_framesource.h>
#include <imp/imp_log.h>
#include <imp/imp_encoder.h>
#include <imp/imp_osd.h>
#include <imp/imp_utils.h>
#include <imp/imp_isp.h>

#include "sample-common.h"

#ifdef SUPPORT_RGB555LE
#include "bgramapinfo_rgb555le.h"
#else
#include "bgramapinfo.h"
#endif

#define TAG "Sample-Encoder-resolution-change"

#define OSD_LETTER_NUM 20

extern struct chn_conf chn[];

IMPRgnHandle g_prHander[2];

static const int S_RC_METHOD = ENC_RC_MODE_SMART;

/* System init */
IMPSensorInfo sensor_info;
static int my_sample_system_init()
{
	int ret = 0;

	memset(&sensor_info, 0, sizeof(IMPSensorInfo));
	memcpy(sensor_info.name, SENSOR_NAME, sizeof(SENSOR_NAME));
	sensor_info.cbus_type = SENSOR_CUBS_TYPE;
	memcpy(sensor_info.i2c.type, SENSOR_NAME, sizeof(SENSOR_NAME));
	sensor_info.i2c.addr = SENSOR_I2C_ADDR;

	IMP_LOG_DBG(TAG, "sample_system_init start\n");

	ret = IMP_ISP_Open();
	if(ret < 0){
		IMP_LOG_ERR(TAG, "failed to open ISP\n");
		return -1;
	}

	ret = IMP_ISP_AddSensor(&sensor_info);
	if(ret < 0){
		IMP_LOG_ERR(TAG, "failed to AddSensor\n");
		return -1;
	}

	ret = IMP_ISP_EnableSensor();
	if(ret < 0){
		IMP_LOG_ERR(TAG, "failed to EnableSensor\n");
		return -1;
	}

	ret = IMP_System_Init();
	if(ret < 0){
		IMP_LOG_ERR(TAG, "IMP_System_Init failed\n");
		return -1;
	}

	/* enable turning, to debug graphics */
	ret = IMP_ISP_EnableTuning();
	if(ret < 0){
		IMP_LOG_ERR(TAG, "IMP_ISP_EnableTuning failed\n");
		return -1;
	}

	ret = IMP_ISP_Tuning_SetSensorFPS(SENSOR_FRAME_RATE_NUM, SENSOR_FRAME_RATE_DEN);
	if (ret < 0){
		IMP_LOG_ERR(TAG, "failed to set sensor fps\n");
		return -1;
	}

	IMP_LOG_DBG(TAG, "ImpSystemInit success\n");

	return 0;
}

/* System exit */
static int my_sample_system_exit()
{
	int ret = 0;

	IMP_LOG_DBG(TAG, "sample_system_exit start\n");

	IMP_System_Exit();

	ret = IMP_ISP_DisableSensor();
	if(ret < 0){
		IMP_LOG_ERR(TAG, "failed to EnableSensor\n");
		return -1;
	}

	ret = IMP_ISP_DelSensor(&sensor_info);
	if(ret < 0){
		IMP_LOG_ERR(TAG, "failed to AddSensor\n");
		return -1;
	}

	ret = IMP_ISP_DisableTuning();
	if(ret < 0){
		IMP_LOG_ERR(TAG, "IMP_ISP_DisableTuning failed\n");
		return -1;
	}

	if(IMP_ISP_Close()){
		IMP_LOG_ERR(TAG, "failed to open ISP\n");
		return -1;
	}

	IMP_LOG_DBG(TAG, " sample_system_exit success\n");

	return 0;
}

/* FrameSource stream on */
static int my_sample_framesource_streamon()
{
	int ret = 0, i = 0;
	/* Enable channels */
	for (i = 0; i < FS_CHN_NUM; i++) {
		if (chn[i].enable){
			ret = IMP_FrameSource_EnableChn(chn[i].index);
			if (ret < 0) {
				IMP_LOG_ERR(TAG, "IMP_FrameSource_EnableChn(%d) error: %d\n", ret, chn[i].index);
				return -1;
			}
		}
	}

	return 0;
}

/* FrameSource stream off */
static int my_sample_framesource_streamoff()
{
	int ret = 0, i = 0;
	/* Disable channels */
	for (i = 0; i < FS_CHN_NUM; i++) {
		if (chn[i].enable){
			ret = IMP_FrameSource_DisableChn(chn[i].index);
			if (ret < 0) {
				IMP_LOG_ERR(TAG, "IMP_FrameSource_DisableChn(%d) error: %d\n", ret, chn[i].index);
				return -1;
			}
		}
	}
	return 0;
}

/* FrameSource init */
static int my_sample_framesource_init()
{
	int i, ret;

	for (i = 0; i <  FS_CHN_NUM; i++) {
		if (chn[i].enable) {
			ret = IMP_FrameSource_CreateChn(chn[i].index, &chn[i].fs_chn_attr);
			if(ret < 0){
				IMP_LOG_ERR(TAG, "IMP_FrameSource_CreateChn(chn%d) error !\n", chn[i].index);
				return -1;
			}

			ret = IMP_FrameSource_SetChnAttr(chn[i].index, &chn[i].fs_chn_attr);
			if (ret < 0) {
				IMP_LOG_ERR(TAG, "IMP_FrameSource_SetChnAttr(chn%d) error !\n",  chn[i].index);
				return -1;
			}
		}
	}

	return 0;
}

/* FrameSource exit */
static int my_sample_framesource_exit()
{
	int ret,i;

	for (i = 0; i <  FS_CHN_NUM; i++) {
		if (chn[i].enable) {
			/*Destroy channel i*/
			ret = IMP_FrameSource_DestroyChn(i);
			if (ret < 0) {
				IMP_LOG_ERR(TAG, "IMP_FrameSource_DestroyChn() error: %d\n", ret);
				return -1;
			}
		}
	}
	return 0;
}

/* Encoder create and register channel */
static int encoder_init(int gr_id, int chn_id)
{
	int  ret;
	IMPEncoderAttr *enc_attr;
	IMPEncoderRcAttr *rc_attr;
	IMPFSChnAttr *imp_chn_attr_tmp;
	IMPEncoderCHNAttr channel_attr;

	imp_chn_attr_tmp = &chn[gr_id].fs_chn_attr;
	memset(&channel_attr, 0, sizeof(IMPEncoderCHNAttr));
	enc_attr = &channel_attr.encAttr;
	enc_attr->enType = PT_H264;
	enc_attr->bufSize = 0;
	enc_attr->profile = 1;
	enc_attr->picWidth = imp_chn_attr_tmp->picWidth;
	enc_attr->picHeight = imp_chn_attr_tmp->picHeight;
	rc_attr = &channel_attr.rcAttr;
	rc_attr->outFrmRate.frmRateNum = imp_chn_attr_tmp->outFrmRateNum;
	rc_attr->outFrmRate.frmRateDen = imp_chn_attr_tmp->outFrmRateDen;
	rc_attr->maxGop = 2 * rc_attr->outFrmRate.frmRateNum / rc_attr->outFrmRate.frmRateDen;
	if (S_RC_METHOD == ENC_RC_MODE_CBR) {
		rc_attr->attrRcMode.rcMode = ENC_RC_MODE_CBR;
		rc_attr->attrRcMode.attrH264Cbr.outBitRate = (double)2000.0 * (imp_chn_attr_tmp->picWidth * imp_chn_attr_tmp->picHeight) / (1280 * 720);
		rc_attr->attrRcMode.attrH264Cbr.maxQp = 45;
		rc_attr->attrRcMode.attrH264Cbr.minQp = 15;
		rc_attr->attrRcMode.attrH264Cbr.iBiasLvl = 0;
		rc_attr->attrRcMode.attrH264Cbr.frmQPStep = 3;
		rc_attr->attrRcMode.attrH264Cbr.gopQPStep = 15;
		rc_attr->attrRcMode.attrH264Cbr.adaptiveMode = false;
		rc_attr->attrRcMode.attrH264Cbr.gopRelation = false;

		rc_attr->attrHSkip.hSkipAttr.skipType = IMP_Encoder_STYPE_N1X;
		rc_attr->attrHSkip.hSkipAttr.m = 0;
		rc_attr->attrHSkip.hSkipAttr.n = 0;
		rc_attr->attrHSkip.hSkipAttr.maxSameSceneCnt = 0;
		rc_attr->attrHSkip.hSkipAttr.bEnableScenecut = 0;
		rc_attr->attrHSkip.hSkipAttr.bBlackEnhance = 0;
		rc_attr->attrHSkip.maxHSkipType = IMP_Encoder_STYPE_N1X;
	} else if (S_RC_METHOD == ENC_RC_MODE_VBR) {
		rc_attr->attrRcMode.rcMode = ENC_RC_MODE_VBR;
		rc_attr->attrRcMode.attrH264Vbr.maxQp = 45;
		rc_attr->attrRcMode.attrH264Vbr.minQp = 15;
		rc_attr->attrRcMode.attrH264Vbr.staticTime = 2;
		rc_attr->attrRcMode.attrH264Vbr.maxBitRate = (double)2000.0 * (imp_chn_attr_tmp->picWidth * imp_chn_attr_tmp->picHeight) / (1280 * 720);
		rc_attr->attrRcMode.attrH264Vbr.iBiasLvl = 0;
		rc_attr->attrRcMode.attrH264Vbr.changePos = 80;
		rc_attr->attrRcMode.attrH264Vbr.qualityLvl = 2;
		rc_attr->attrRcMode.attrH264Vbr.frmQPStep = 3;
		rc_attr->attrRcMode.attrH264Vbr.gopQPStep = 15;
		rc_attr->attrRcMode.attrH264Vbr.gopRelation = false;

		rc_attr->attrHSkip.hSkipAttr.skipType = IMP_Encoder_STYPE_N1X;
		rc_attr->attrHSkip.hSkipAttr.m = 0;
		rc_attr->attrHSkip.hSkipAttr.n = 0;
		rc_attr->attrHSkip.hSkipAttr.maxSameSceneCnt = 0;
		rc_attr->attrHSkip.hSkipAttr.bEnableScenecut = 0;
		rc_attr->attrHSkip.hSkipAttr.bBlackEnhance = 0;
		rc_attr->attrHSkip.maxHSkipType = IMP_Encoder_STYPE_N1X;
	} else if (S_RC_METHOD == ENC_RC_MODE_SMART) {
		rc_attr->attrRcMode.rcMode = ENC_RC_MODE_SMART;
		rc_attr->attrRcMode.attrH264Smart.maxQp = 45;
		rc_attr->attrRcMode.attrH264Smart.minQp = 15;
		rc_attr->attrRcMode.attrH264Smart.staticTime = 2;
		rc_attr->attrRcMode.attrH264Smart.maxBitRate = (double)2000.0 * (imp_chn_attr_tmp->picWidth * imp_chn_attr_tmp->picHeight) / (1280 * 720);
		rc_attr->attrRcMode.attrH264Smart.iBiasLvl = 0;
		rc_attr->attrRcMode.attrH264Smart.changePos = 80;
		rc_attr->attrRcMode.attrH264Smart.qualityLvl = 2;
		rc_attr->attrRcMode.attrH264Smart.frmQPStep = 3;
		rc_attr->attrRcMode.attrH264Smart.gopQPStep = 15;
		rc_attr->attrRcMode.attrH264Smart.gopRelation = false;

		rc_attr->attrHSkip.hSkipAttr.skipType = IMP_Encoder_STYPE_N1X;
		rc_attr->attrHSkip.hSkipAttr.m = rc_attr->maxGop - 1;
		rc_attr->attrHSkip.hSkipAttr.n = 1;
		rc_attr->attrHSkip.hSkipAttr.maxSameSceneCnt = 6;
		rc_attr->attrHSkip.hSkipAttr.bEnableScenecut = 0;
		rc_attr->attrHSkip.hSkipAttr.bBlackEnhance = 0;
		rc_attr->attrHSkip.maxHSkipType = IMP_Encoder_STYPE_N1X;
	} else { /* fixQp */
		rc_attr->attrRcMode.rcMode = ENC_RC_MODE_FIXQP;
		rc_attr->attrRcMode.attrH264FixQp.qp = 42;

		rc_attr->attrHSkip.hSkipAttr.skipType = IMP_Encoder_STYPE_N1X;
		rc_attr->attrHSkip.hSkipAttr.m = 0;
		rc_attr->attrHSkip.hSkipAttr.n = 0;
		rc_attr->attrHSkip.hSkipAttr.maxSameSceneCnt = 0;
		rc_attr->attrHSkip.hSkipAttr.bEnableScenecut = 0;
		rc_attr->attrHSkip.hSkipAttr.bBlackEnhance = 0;
		rc_attr->attrHSkip.maxHSkipType = IMP_Encoder_STYPE_N1X;
	}


	ret = IMP_Encoder_CreateChn(chn_id, &channel_attr);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_Encoder_CreateChn(%d) error !\n", 0);
		return -1;
	}

	ret = IMP_Encoder_RegisterChn(gr_id, chn_id);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_Encoder_RegisterChn(%d, %d) error: %d\n",
				gr_id, chn_id, ret);
		return -1;
	}

	return 0;
}

/* Encoder channel exit */
static int encoder_chn_exit(int encChn)
{
	int ret;
	IMPEncoderCHNStat chn_stat;
	ret = IMP_Encoder_Query(encChn, &chn_stat);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_Encoder_Query(%d) error: %d\n",
				encChn, ret);
		return -1;
	}

	if (chn_stat.registered) {
		ret = IMP_Encoder_UnRegisterChn(encChn);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "IMP_Encoder_UnRegisterChn(%d) error: %d\n",
					encChn, ret);
			return -1;
		}
		ret = IMP_Encoder_DestroyChn(encChn);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "IMP_Encoder_DestroyChn(%d) error: %d\n",
					encChn, ret);
			return -1;
		}
	}

	return 0;
}

/* OSD show region */
static int osd_show(void)
{
	int ret, i = 0;

	for (i = 0; i < FS_CHN_NUM; i++){
		ret = IMP_OSD_ShowRgn(g_prHander[i], i, 1);
		if (ret != 0) {
			IMP_LOG_ERR(TAG, "IMP_OSD_ShowRgn() timeStamp error\n");
			return -1;
		}
	}

	return 0;
}

/* OSD show timestamp thread function */
static void *update_thread(void *p)
{
	int ret;

	/*generate time*/
	char DateStr[40];
	time_t currTime;
	struct tm *currDate;
	unsigned i = 0, j = 0;
	void *dateData = NULL;
	uint32_t *data = p;
	IMPOSDRgnAttrData rAttrData;

	ret = osd_show();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "OSD show error\n");
		return NULL;
	}

	while(1) {
		int penpos_t = 0;
		int fontadv = 0;

		time(&currTime);
		currDate = localtime(&currTime);
		memset(DateStr, 0, 40);
		strftime(DateStr, 40, "%Y-%m-%d %I:%M:%S", currDate);
		for (i = 0; i < OSD_LETTER_NUM; i++) {
			switch(DateStr[i]) {
				case '0' ... '9':
					dateData = (void *)gBgramap[DateStr[i] - '0'].pdata;
					fontadv = gBgramap[DateStr[i] - '0'].width;
					penpos_t += gBgramap[DateStr[i] - '0'].width;
					break;
				case '-':
					dateData = (void *)gBgramap[10].pdata;
					fontadv = gBgramap[10].width;
					penpos_t += gBgramap[10].width;
					break;
				case ' ':
					dateData = (void *)gBgramap[11].pdata;
					fontadv = gBgramap[11].width;
					penpos_t += gBgramap[11].width;
					break;
				case ':':
					dateData = (void *)gBgramap[12].pdata;
					fontadv = gBgramap[12].width;
					penpos_t += gBgramap[12].width;
					break;
				default:
					break;
			}
#ifdef SUPPORT_RGB555LE
			for (j = 0; j < OSD_REGION_HEIGHT; j++) {
				memcpy((void *)((uint16_t *)data + j*OSD_LETTER_NUM*OSD_REGION_WIDTH + penpos_t),
						(void *)((uint16_t *)dateData + j*fontadv), fontadv*sizeof(uint16_t));
			}
#else
			for (j = 0; j < OSD_REGION_HEIGHT; j++) {
				memcpy((void *)((uint32_t *)data + j*OSD_LETTER_NUM*OSD_REGION_WIDTH + penpos_t),
						(void *)((uint32_t *)dateData + j*fontadv), fontadv*sizeof(uint32_t));
			}

#endif
		}
		rAttrData.picData.pData = data;

		IMP_OSD_UpdateRgnAttrData(g_prHander[0], &rAttrData);
		IMP_OSD_UpdateRgnAttrData(g_prHander[1], &rAttrData);

		sleep(1);
	}

	return NULL;
}

static int save_stream(int fd, IMPEncoderStream *stream)
{
	int ret, i, nr_pack = stream->packCount;

	for (i = 0; i < nr_pack; i++) {
		ret = write(fd, (void *)stream->pack[i].virAddr,stream->pack[i].length);
		if (ret != stream->pack[i].length){
			IMP_LOG_ERR(TAG,"stream write error:%s\n", strerror(errno));
			return -1;
		}
	}

	return 0;
}

static int get_h264_stream(int nr_frames, int chn)
{
	int ret;

	time_t now;
	time(&now);
	struct tm *now_tm;
	now_tm = localtime(&now);
	char now_str[32];
	strftime(now_str, 40, "%Y%m%d%I%M%S", now_tm);

	char stream_path[128];
	sprintf(stream_path, "%s/stream%d-%s.h264",STREAM_FILE_PATH_PREFIX, chn, now_str);
	IMP_LOG_DBG(TAG, "Open Stream file %s ", stream_path);
	int stream_fd = open(stream_path, O_RDWR | O_CREAT | O_TRUNC, 0777);
	if (stream_fd < 0) {
		IMP_LOG_ERR(TAG, "failed: %s\n", strerror(errno));
		return -1;
	}
	IMP_LOG_DBG(TAG, "OK\n");

	int i;
	for (i = 0; i < nr_frames; i++) {
		/* Polling H264 Stream, set timeout as 1000msec */
		ret = IMP_Encoder_PollingStream(chn, 1000);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "Polling stream timeout\n");
			continue;
		}

		IMPEncoderStream stream;
		/* Get H264 Stream */
		ret = IMP_Encoder_GetStream(chn, &stream, 1);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "IMP_Encoder_GetStream() failed\n");
			return -1;
		}

		ret = save_stream(stream_fd, &stream);
		if (ret < 0) {
			close(stream_fd);
			return ret;
		}

		IMP_Encoder_ReleaseStream(chn, &stream);
	}

	close(stream_fd);

	return 0;
}

/* Encoder get h264 stream thread function */
static void *h264_stream_thread(void *data)
{
	int  ret;
	int chn = *(int *)data;
	ret = IMP_Encoder_StartRecvPic(chn);
	if (ret < 0){
		IMP_LOG_ERR(TAG, "IMP_Encoder_StartRecvPic(%d) failed\n", 1);
		return NULL;
	}
	ret = get_h264_stream(100, chn);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "Get H264 stream failed\n");
		return NULL;
	}
	ret = IMP_Encoder_StopRecvPic(chn);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_Encoder_StopRecvPic() failed\n");
		return NULL;
	}

	return 0;
}

/* OSD region init */
static IMPRgnHandle my_sample_osd_init(int grpNum)
{
	int ret;
	IMPRgnHandle rHanderFont;

	rHanderFont = IMP_OSD_CreateRgn(NULL);
	if (rHanderFont == INVHANDLE) {
		IMP_LOG_ERR(TAG, "IMP_OSD_CreateRgn TimeStamp error !\n");
		return -1;
	}

	ret = IMP_OSD_RegisterRgn(rHanderFont, grpNum, NULL);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IVS IMP_OSD_RegisterRgn failed\n");
		return -1;
	}

	IMPOSDRgnAttr rAttrFont;
	memset(&rAttrFont, 0, sizeof(IMPOSDRgnAttr));
	rAttrFont.type = OSD_REG_PIC;
	rAttrFont.rect.p0.x = 10;
	rAttrFont.rect.p0.y = 10;
	rAttrFont.rect.p1.x = rAttrFont.rect.p0.x + 20 * OSD_REGION_WIDTH- 1;   //p0 is start，and p1 well be epual p0+width(or heigth)-1
	rAttrFont.rect.p1.y = rAttrFont.rect.p0.y + OSD_REGION_HEIGHT - 1;
#ifdef SUPPORT_RGB555LE
	rAttrFont.fmt = PIX_FMT_RGB555LE;
#else
	rAttrFont.fmt = PIX_FMT_BGRA;
#endif
	rAttrFont.data.picData.pData = NULL;
	ret = IMP_OSD_SetRgnAttr(rHanderFont, &rAttrFont);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_OSD_SetRgnAttr TimeStamp error !\n");
		return -1;
	}

	IMPOSDGrpRgnAttr grAttrFont;

	if (IMP_OSD_GetGrpRgnAttr(rHanderFont, grpNum, &grAttrFont) < 0) {
		IMP_LOG_ERR(TAG, "IMP_OSD_GetGrpRgnAttr Logo error !\n");
		return -1;

	}
	memset(&grAttrFont, 0, sizeof(IMPOSDGrpRgnAttr));
	grAttrFont.show = 0;

	/* Disable Font global alpha, only use pixel alpha. */
	grAttrFont.gAlphaEn = 1;
	grAttrFont.fgAlhpa = 0xff;
	grAttrFont.layer = 3;
	if (IMP_OSD_SetGrpRgnAttr(rHanderFont, grpNum, &grAttrFont) < 0) {
		IMP_LOG_ERR(TAG, "IMP_OSD_SetGrpRgnAttr Logo error !\n");
		return -1;
	}

	ret = IMP_OSD_Start(grpNum);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_OSD_Start TimeStamp, Logo, Cover and Rect error !\n");
		return -1;
	}

	return rHanderFont;
}

/* OSD exit */
static int my_sample_osd_exit(IMPRgnHandle prHander,int grpNum)
{
	int ret;

	ret = IMP_OSD_ShowRgn(prHander, grpNum, 0);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_OSD_ShowRgn close timeStamp error\n");
	}

	ret = IMP_OSD_UnRegisterRgn(prHander, grpNum);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_OSD_UnRegisterRgn timeStamp error\n");
	}


	IMP_OSD_DestroyRgn(prHander);

	return 0;
}

/* called in main for reducing tne lines of main */
static int my_sample_main(void *osdbuf)
{
	int ret = 0;
	int i = 0;
	pthread_t tid;
	/* step.2 FrameSource init */
	ret = my_sample_framesource_init();
	if (ret < 0){
		IMP_LOG_ERR(TAG, "FrameSource init failed\n");
		return -1;
	}

	for(i = 0;i < FS_CHN_NUM;i++){
		if (chn[i].enable == 1){
			/* step.3 Encoder init */
			ret = encoder_init(chn[i].index, i);
			if (ret < 0){
				IMP_LOG_ERR(TAG, "Encoder init failed\n");
				return -1;
			}


			/* step.4 OSD init */
			g_prHander[i] = my_sample_osd_init(i);
			if (g_prHander[i] < 0){
				IMP_LOG_ERR(TAG, "OSD init failed\n");
				return -1;
			}
		}
	}
	for(i = 0;i < FS_CHN_NUM;i++){
		if (chn[i].enable == 1){
			ret = IMP_Encoder_CreateGroup(chn[i].index);
			if (ret < 0){
				IMP_LOG_ERR(TAG, "IMP_Encoder_CreateGroup(%d) error!\n",chn[i].index);
				return -1;
			}

			if (IMP_OSD_CreateGroup(i) < 0){
				IMP_LOG_ERR(TAG, "IMP_OSD_CreateGroup(%d) error!\n",i);
				return -1;
			}

			/* step.5 Bind */
			IMPCell osdcell = {DEV_ID_OSD,i,0};
			ret = IMP_System_Bind(&chn[i].framesource_chn,&osdcell);
			if (ret < 0){
				IMP_LOG_ERR(TAG, "Bind FrameSource channal%d and OSD failed\n",i);
				return -1;
			}

			ret = IMP_System_Bind(&osdcell,&chn[i].imp_encoder);
			if (ret < 0){
				IMP_LOG_ERR(TAG, "Bind OSD and Encoder failed\n");
				return -1;
			}
		}
	}

	ret = pthread_create(&tid, NULL, update_thread, osdbuf);
	if (ret) {
		IMP_LOG_ERR(TAG, "thread create error\n");
		return -1;
	}

	/* step 6. Stream On */
	IMP_FrameSource_SetFrameDepth(0,0);
	ret = my_sample_framesource_streamon();
	if (ret < 0){
		IMP_LOG_ERR(TAG, "ImpStreamOn failed\n");
		return -1;
	}

	pthread_t tid0;
	ret = pthread_create(&tid0, NULL, h264_stream_thread, &chn[0].index);
	if(ret){
		IMP_LOG_ERR(TAG, "h264 stream create error\n");
		return -1;
	}

	pthread_t tid1;
	ret = pthread_create(&tid1, NULL, h264_stream_thread, &chn[1].index);
	if(ret){
		IMP_LOG_ERR(TAG, "h264 stream create error\n");
		return -1;
	}

	pthread_join(tid0, NULL);
	pthread_join(tid1, NULL);

	/* Exit sequence as follow */
	/* Step. a Stream Off */
	ret = my_sample_framesource_streamoff();
	if (ret < 0 ){
		IMP_LOG_ERR(TAG, "ImpStreamOff failed\n");
		return -1;
	}

	pthread_cancel(tid);
	pthread_join(tid,NULL);
	for(i = 0;i < FS_CHN_NUM;i++){
		if (chn[i].enable == 1){
			ret = my_sample_osd_exit(g_prHander[i],i);
			if (ret < 0){
				IMP_LOG_ERR(TAG,"OSD exit failed\n");
				return -1;
			}

			/* Step.d Encoder exit */
			ret = encoder_chn_exit(i);
			if (ret < 0){
				IMP_LOG_ERR(TAG, "Encoder exit failed\n");
				return -1;
			}
		}
	}
	for (i = 0;i < FS_CHN_NUM;i++){
		if (chn[i].enable == 1){
			IMPCell osdcell = {DEV_ID_OSD,i,0};
			/* Step. b UnBind */
			ret = IMP_System_UnBind(&osdcell,&chn[i].imp_encoder);
			if (ret < 0){
				IMP_LOG_ERR(TAG, "UnBind OSD and Encoder failed\n");
				ret = -1;
			}

			ret = IMP_System_UnBind(&chn[i].framesource_chn,&osdcell);
			if (ret < 0){
				IMP_LOG_ERR(TAG, "UnBind FrameSource and OSD failed\n");
				ret = -1;
			}

			/* Step.c OSD destroy */
			ret = IMP_OSD_DestroyGroup(i);
			if (ret < 0) {
				IMP_LOG_ERR(TAG, "IMP_OSD_DestroyGroup(0) error\n");
				ret = -1;
			}

			ret = IMP_Encoder_DestroyGroup(chn[i].index);
			if (ret < 0) {
				IMP_LOG_ERR(TAG, "IMP_Encoder_DestroyGroup(0) error: %d\n", ret);
				ret = -1;
			}
		}
	}

	/* Step.e FrameSource exit */
	ret = my_sample_framesource_exit();
	if (ret < 0){
		IMP_LOG_ERR(TAG, "FrameSource exit failed\n");
		ret = -1;
	}

	return 0;
}

int main (int argc,char *argv[])
{
	int ret;
	int i;

	/*step.1 System init */
	ret = my_sample_system_init();
	if (ret < 0){
		IMP_LOG_ERR(TAG, "IMP_System_init() failed\n");
		return -1;
	}

#ifdef SUPPORT_RGB555LE
	uint32_t *timeStampData = malloc(OSD_LETTER_NUM * OSD_REGION_HEIGHT * OSD_REGION_WIDTH * sizeof(uint16_t));
#else
	uint32_t *timeStampData = malloc(OSD_LETTER_NUM * OSD_REGION_HEIGHT * OSD_REGION_WIDTH * sizeof(uint32_t));
#endif

	if (timeStampData == NULL) {
		IMP_LOG_ERR(TAG, "valloc timeStampData error\n");
		return -1;
	}

	/* get 1920x1080 resolution h264 stream in main and second h264 stream */
	ret = my_sample_main(timeStampData);
	if (ret < 0){
		IMP_LOG_ERR(TAG, "my_sample_main() failed\n");
		ret = -1;
		goto exit;
	}

	chn[0].fs_chn_attr.scaler.enable = 1;
	chn[0].fs_chn_attr.scaler.outwidth = 1280;
	chn[0].fs_chn_attr.scaler.outheight = 720;
	chn[0].fs_chn_attr.picWidth = 1280;
	chn[0].fs_chn_attr.picHeight = 720;

	/* get 1280x720 resolution h264 stream in main and second h264 stream */
	ret = my_sample_main(timeStampData);
	if (ret < 0){
		IMP_LOG_ERR(TAG, "my_sample_main() failed\n");
		ret = -1;
		goto exit;
	}

	ret = my_sample_system_exit();
	if (ret < 0){
		IMP_LOG_ERR(TAG, "sample_system_exit() failed\n");
		ret = -1;
		goto exit;
	}
exit:
	if(timeStampData)
		free(timeStampData);
	return ret;
}






