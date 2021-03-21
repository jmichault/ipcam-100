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
// C++ header

#ifndef _WEBCAM_JPEG_DEVICE_SOURCE_HH
#define _WEBCAM_JPEG_DEVICE_SOURCE_HH

#include "liveMedia/JPEGVideoSource.hh"
#include "JpegFrameParser.hh"

#include <exception>

#define MAX_JPEG_FILE_SZ 100000

#define IMP_BUFFER_SIZE 200000

class DeviceException : public std::exception {

};

class ImpJpegVideoDeviceSource : public JPEGVideoSource {
public:
    static ImpJpegVideoDeviceSource *createNew(UsageEnvironment &env,
                                               int canal );
    // "timePerFrame" is in microseconds

protected:
    ImpJpegVideoDeviceSource(UsageEnvironment &env,
                             int params);

    // called only by createNew()
    virtual ~ImpJpegVideoDeviceSource();

private:
    // redefined virtual functions:
    virtual void doGetNextFrame();

    virtual u_int8_t type();

    virtual u_int8_t qFactor();

    virtual u_int8_t width();

    virtual u_int8_t height();

    virtual u_int8_t const *quantizationTables(u_int8_t &precision, u_int16_t &length);

private:

    int initDevice(int fd);

    struct buffer {
        void *start;
        size_t length;
    };

    size_t jpeg_to_rtp(void *to, void *from, size_t len);

private:
    int fFd;
    unsigned fTimePerFrame;
    struct timeval fLastCaptureTime;
    struct buffer *fBuffers;
    unsigned int fNbuffers;
    JpegFrameParser parser;


};

#endif // _WEBCAM_JPEG_DEVICE_SOURCE_HH
