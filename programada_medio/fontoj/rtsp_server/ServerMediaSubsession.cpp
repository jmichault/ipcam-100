/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** ServerMediaSubsession.cpp
** 
** -------------------------------------------------------------------------*/

#include <sstream>

// libv4l2
#include <linux/videodev2.h>

// live555
#include <BasicUsageEnvironment/BasicUsageEnvironment.hh>
#include <groupsock/GroupsockHelper.hh>
#include <liveMedia/Base64.hh>
#include "liveMedia/JPEGVideoRTPSink.hh"

// project
#include "ServerMediaSubsession.h"
#include "V4l2DeviceSource.h"
#include "MJPEGVideoSource.h"
#include "ImpJpegVideoDeviceSource.h"
#define SAMPLE_RATE 8000 // 8K

// ---------------------------------
//   BaseServerMediaSubsession
// ---------------------------------
FramedSource *BaseServerMediaSubsession::createSource(UsageEnvironment &env, FramedSource *videoES, int format) {
    FramedSource *source = NULL;
    switch (format) {
        case V4L2_PIX_FMT_H264 :
            source = H264VideoStreamDiscreteFramer::createNew(env, videoES);
            break;

        case V4L2_PIX_FMT_MJPEG:

            fprintf(stderr,"avant source = ImpJpegVideoDeviceSource::createNew(env, params)\n");
            source = ImpJpegVideoDeviceSource::createNew(env, 0);


            //source = videoES;
            //source = MJPEGVideoSource::createNew(env, videoES);
            break;

#ifdef    V4L2_PIX_FMT_VP8
            case V4L2_PIX_FMT_VP8  : source = videoES; break;
#endif
        case WA_PCMA :
            source = videoES;
            break;
    }
    return source;
}

RTPSink *BaseServerMediaSubsession::createSink(UsageEnvironment &env, Groupsock *rtpGroupsock,
                                               unsigned char rtpPayloadTypeIfDynamic, int format) {
    RTPSink *videoSink = NULL;
    switch (format) {
        case V4L2_PIX_FMT_H264 :
            videoSink = H264VideoRTPSink::createNew(env, rtpGroupsock, rtpPayloadTypeIfDynamic);
            break;

        case V4L2_PIX_FMT_MJPEG :
            videoSink = JPEGVideoRTPSink::createNew(env, rtpGroupsock);
            break;

#ifdef    V4L2_PIX_FMT_VP8
            case V4L2_PIX_FMT_VP8  : videoSink = VP8VideoRTPSink::createNew(env, rtpGroupsock,rtpPayloadTypeIfDynamic); break;
#endif
        case WA_PCMA :
            char const *mimeType;
            unsigned char payloadFormatCode = 8; // by default, unless a static RTP payload type can be used
            unsigned const samplingFrequency = SAMPLE_RATE;
            unsigned char const numChannels = 1;
            if (format == WA_PCMU) {
                mimeType = "PCMU";
                if (samplingFrequency == 8000 && numChannels == 1) {
                    payloadFormatCode = 0; // a static RTP payload type
                }
            } else if (format == WA_PCMA) {
                mimeType = "PCMA";
                if (samplingFrequency == 8000 && numChannels == 1) {
                    payloadFormatCode = 8; // a static RTP payload type
                }
            } else if (format == WA_IMA_ADPCM) {
                mimeType = "DVI4";
                // Use a static payload type, if one is defined:
                if (numChannels == 1) {
                    if (samplingFrequency == 8000) {
                        payloadFormatCode = 5; // a static RTP payload type
                    } else if (samplingFrequency == 16000) {
                        payloadFormatCode = 6; // a static RTP payload type
                    } else if (samplingFrequency == 11025) {
                        payloadFormatCode = 16; // a static RTP payload type
                    } else if (samplingFrequency == 22050) {
                        payloadFormatCode = 17; // a static RTP payload type
                    }
                }
            } else { //unknown format
                //*env << "Unknown audio format code \"" << format << "\" in WAV file header\n";
                fprintf(stderr, "Error Unknown audio format code \n");

            }

            fprintf(stderr, "create audio sink %d %d %s \n", payloadFormatCode, samplingFrequency, mimeType);

            videoSink = SimpleRTPSink::createNew(env, rtpGroupsock, payloadFormatCode, samplingFrequency,
                                                 "audio", mimeType, numChannels);
            break;
    }
    return videoSink;
}

char const *BaseServerMediaSubsession::getAuxLine(V4L2DeviceSource *source, unsigned char rtpPayloadType) {
    const char *auxLine = NULL;
    if (source) {
        std::ostringstream os;
        os << "a=fmtp:" << int(rtpPayloadType) << " ";
        os << source->getAuxLine();
        os << "\r\n";
        auxLine = strdup(os.str().c_str());
        printf("auxLine %s\n", auxLine); //haoweilo
    }
    return auxLine;
}

// -----------------------------------------
//    ServerMediaSubsession for Unicast
// -----------------------------------------
UnicastServerMediaSubsession *
UnicastServerMediaSubsession::createNew(UsageEnvironment &env, StreamReplicator *replicator, int format) {
    return new UnicastServerMediaSubsession(env, replicator, format);
}

FramedSource *UnicastServerMediaSubsession::createNewStreamSource(unsigned clientSessionId, unsigned &estBitrate) {
    FramedSource *source = m_replicator->createStreamReplica();
    fprintf(stderr,"UnicastServerMediaSubsession::createNewStreamSource\n");
    estBitrate = 50;
    return createSource(envir(), source, m_format);
}


RTPSink *UnicastServerMediaSubsession::createNewRTPSink(Groupsock *rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic,
                                                        FramedSource *inputSource) {
    //printf("UnicastServerMediaSubsession::createNewRTPSink\n");
    return createSink(envir(), rtpGroupsock, rtpPayloadTypeIfDynamic, m_format);
}

char const *UnicastServerMediaSubsession::getAuxSDPLine(RTPSink *rtpSink, FramedSource *inputSource) {
    return this->getAuxLine(dynamic_cast<V4L2DeviceSource *>(m_replicator->inputSource()), rtpSink->rtpPayloadType());
}
