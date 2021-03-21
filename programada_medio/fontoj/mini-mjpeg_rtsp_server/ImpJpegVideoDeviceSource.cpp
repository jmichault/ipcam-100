/*
 Copyright (C) 2015, Kyle Zhou <kyle.zhou at live.com>
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
// Webcam MJPEG camera input device
// Implementation

#include "ImpJpegVideoDeviceSource.h"
#include "imp_komuna.h"

#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include "../rtspserver-tools/sharedmem.h"

#include "JpegFrameParser.hh"
#include <algorithm>
#include <iostream>

#define KOMUNA_KANALO 0

ImpJpegVideoDeviceSource *
ImpJpegVideoDeviceSource::createNew(UsageEnvironment &env,
                                    int canal) {
    try {
        return new ImpJpegVideoDeviceSource(env, canal);
    } catch (DeviceException) {
        return NULL;
    }
}

unsigned char * bufferStream=NULL;

int ImpJpegVideoDeviceSource::initDevice(int canal) {
    fprintf(stderr, "début Init Device...\n");
    fMaxSize = fs_chn_attrs[canal].crop.width * fs_chn_attrs[canal].crop.height;
  bufferStream = (unsigned char *) malloc(fMaxSize);
    unsigned timePerFrame = 1000000 * fs_chn_attrs[canal].outFrmRateDen /  fs_chn_attrs[canal].outFrmRateNum;
    fTimePerFrame = timePerFrame;
    fprintf(stderr, "fin Init Device...\n");
    return 0;
}

ImpJpegVideoDeviceSource
::ImpJpegVideoDeviceSource(UsageEnvironment &env, int canal)
        : JPEGVideoSource(env)  {
    m_canal=canal;
    if (initDevice(canal)) {
        throw DeviceException();
    }
}

ImpJpegVideoDeviceSource::~ImpJpegVideoDeviceSource() {
}


static struct timezone Idunno;

void ImpJpegVideoDeviceSource::doGetNextFrame() {
    static unsigned long framecount = 0;
    static struct timeval starttime;

    gettimeofday(&fLastCaptureTime, &Idunno);
    if (framecount == 0)
        starttime = fLastCaptureTime;
    framecount++;
    fPresentationTime = fLastCaptureTime;

    int bytesRead = 0;

    /* Polling JPEG Snap, set timeout as 1000msec */
    int ret = IMP_Encoder_PollingStream(m_canal, 1000);
    if (ret >= 0)
    {
      IMPEncoderStream stream;
      /* Get JPEG Snap */
      ret = IMP_Encoder_GetStream(m_canal, &stream, 1);
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
      IMP_Encoder_ReleaseStream(m_canal, &stream);
    }

    if(m_canal == KOMUNA_KANALO)
    {
      // kopio al komuna memoro
      SharedMem &sharedMem = SharedMem::instance();
      sharedMem.copyImage(bufferStream,bytesRead);
    }
    fFrameSize =bytesRead;
    if (parser.parse(bufferStream, bytesRead) == 0)
    { // successful parsing
      unsigned int datlen;
      unsigned char const *dat;
      dat = parser.scandata(datlen);
      if(datlen > fMaxSize)
      {
        fprintf(stderr
                ,"ImpJpegVideoDeviceSource::doGetNextFrame(): read maximum buffer size: %d bytes.  Frame may be truncated (%d).\n"
		,fMaxSize,datlen);
          memcpy(fTo, dat, fMaxSize);
          fNumTruncatedBytes = bytesRead-fMaxSize;
      }
        else
      {
          memcpy(fTo, dat, datlen);
      }
    }





    // Switch to another task, and inform the reader that he has data:
    nextTask() = envir().taskScheduler().scheduleDelayedTask(0,
                                                             (TaskFunc *) FramedSource::afterGetting, this);
}



u_int8_t const *ImpJpegVideoDeviceSource::quantizationTables(u_int8_t &precision, u_int16_t &length) {
    precision = parser.precision();
    return parser.quantizationTables(length);
}

u_int8_t ImpJpegVideoDeviceSource::type() {
    return parser.type();
}

u_int8_t ImpJpegVideoDeviceSource::qFactor() {
    return parser.qFactor();
}

u_int8_t ImpJpegVideoDeviceSource::width() {
    return parser.width();
}

u_int8_t ImpJpegVideoDeviceSource::height() {
    return parser.height();
}