/*
 * sample-Encoder-h264.c
 *
 * Copyright (C) 2014 Ingenic Semiconductor Co.,Ltd
 */


#include <imp/imp_log.h>
#include <imp/imp_common.h>
#include <imp/imp_system.h>
#include <imp/imp_framesource.h>
#include <imp/imp_encoder.h>

#include "sample-common.h"

#define TAG "Sample-Encoder-h264"

extern struct chn_conf chn[];

int sample_snap_fs_bgra(void)
{
	int ret = 0;
	int i = 0;
	FILE *fp;
	IMPFrameInfo *frame;

	fp = fopen("/tmp/snap.bgra", "wb");
	if(fp == NULL) {
		IMP_LOG_ERR(TAG, "%s(%d):open error !\n", __func__, __LINE__);
		return -1;
	}

	for (i = 0; i < 100; i++) {
		/* Snap BGRA */
		ret = IMP_FrameSource_SetFrameDepth(2, 1);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "%s(%d):IMP_FrameSource_SetFrameDepth failed\n", __func__, __LINE__);
			return -1;
		}

		ret = IMP_FrameSource_GetFrame(2, &frame);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "%s(%d):IMP_FrameSource_GetFrame failed\n", __func__, __LINE__);
			return -1;
		}

		if (80 == i) {
			fwrite((void *)frame->virAddr, frame->size, 1, fp);
			fclose(fp);
		}
		IMP_FrameSource_ReleaseFrame(2, frame);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "%s(%d):IMP_FrameSource_ReleaseFrame failed\n", __func__, __LINE__);
			return -1;
		}
		ret = IMP_FrameSource_SetFrameDepth(2, 0);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "%s(%d):IMP_FrameSource_SetFrameDepth failed\n", __func__, __LINE__);
			return -1;
		}
	}
	/* end */
	return 0;
}

extern int IMP_FrameSource_SetSource(int extchnNum, int sourcechnNum);
int main(int argc, char *argv[])
{
	int i, ret;

	/* Step.1 System init */
	ret = sample_system_init();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_System_Init() failed\n");
		return -1;
	}

	/* Step.2 FrameSource init */
	ret = sample_framesource_ext_bgra_init();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "FrameSource init ext bgra failed\n");
		return -1;
	}
	ret = sample_framesource_init();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "FrameSource init failed\n");
		return -1;
	}


	IMP_FrameSource_SetSource(2, 1);
	for (i = 0; i < FS_CHN_NUM; i++) {
		if (chn[i].enable) {
			ret = IMP_Encoder_CreateGroup(chn[i].index);
			if (ret < 0) {
				IMP_LOG_ERR(TAG, "IMP_Encoder_CreateGroup(%d) error !\n", i);
				return -1;
			}
		}
	}

	/* Step.3 Encoder init */
	ret = sample_encoder_init();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "Encoder init failed\n");
		return -1;
	}

	/* Step.4 Bind */
	for (i = 0; i < FS_CHN_NUM; i++) {
		if (chn[i].enable) {
			ret = IMP_System_Bind(&chn[i].framesource_chn, &chn[i].imp_encoder);
			if (ret < 0) {
				IMP_LOG_ERR(TAG, "Bind FrameSource channel%d and Encoder failed\n",i);
				return -1;
			}
		}
	}

	/* Step.5 Stream On */
	ret = sample_framesource_streamon();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "ImpStreamOn failed\n");
		return -1;
	}
	ret = sample_framesource_ext_bgra_streamon();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "ImpStreamOn ext bgra failed\n");
		return -1;
	}


	/* Step.6 Get stream */

#if 1
	ret = sample_snap_fs_bgra();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "Get FS BGRA failed\n");
		return -1;
	}
#endif

	ret = sample_get_h264_stream();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "Get H264 stream failed\n");
		return -1;
	}

	/* Exit sequence as follow */

	/* Step.a Stream Off */
	ret = sample_framesource_streamoff();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "FrameSource StreamOff failed\n");
		return -1;
	}

	ret = sample_framesource_ext_bgra_streamoff();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "FrameSource StreamOff  ext bgra failed\n");
		return -1;
	}

	/* Step.b UnBind */
	for (i = 0; i < FS_CHN_NUM; i++) {
		if (chn[i].enable) {
			ret = IMP_System_UnBind(&chn[i].framesource_chn, &chn[i].imp_encoder);
			if (ret < 0) {
				IMP_LOG_ERR(TAG, "UnBind FrameSource channel%d and Encoder failed\n",i);
				return -1;
			}
		}
	}

	/* Step.c Encoder exit */
	ret = sample_encoder_exit();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "Encoder exit failed\n");
		return -1;
	}

	/* Step.d FrameSource exit */
	ret = sample_framesource_exit();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "FrameSource exit failed\n");
		return -1;
	}
	ret = sample_framesource_ext_bgra_exit();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "FrameSource exit bgra exit failed\n");
		return -1;
	}

	/* Step.e System exit */
	ret = sample_system_exit();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "sample_system_exit() failed\n");
		return -1;
	}

	return 0;
}
