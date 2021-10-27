/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** ServerMediaSubsession.cpp
** 
** -------------------------------------------------------------------------*/

#include <stdlib.h>
#include <string.h>
#ifdef UZI_DMALLOC
#include <dmalloc.h>
#endif
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
#include "ImpJpegVideoDeviceSource.h"
#define SAMPLE_RATE 8000 // 8K

// ---------------------------------
//   BaseServerMediaSubsession
// ---------------------------------
FramedSource *BaseServerMediaSubsession::createSource(UsageEnvironment &env, int canal) {
    FramedSource *source = NULL;
   source = ImpJpegVideoDeviceSource::createNew(env, canal);
    return source;
}

RTPSink *BaseServerMediaSubsession::createSink(UsageEnvironment &env, Groupsock *rtpGroupsock,
                                               unsigned char rtpPayloadTypeIfDynamic, int canal) {
    RTPSink *videoSink = NULL;
            videoSink = JPEGVideoRTPSink::createNew(env, rtpGroupsock);
    return videoSink;
}

// -----------------------------------------
//    ServerMediaSubsession for Unicast
// -----------------------------------------
UnicastServerMediaSubsession *
UnicastServerMediaSubsession::createNew(UsageEnvironment &env, int canal) {
    return new UnicastServerMediaSubsession(env, canal);
}

FramedSource *UnicastServerMediaSubsession::createNewStreamSource(unsigned clientSessionId, unsigned &estBitrate) {
    estBitrate = 50;
    return createSource(envir(), m_canal);
}


RTPSink *UnicastServerMediaSubsession::createNewRTPSink(Groupsock *rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic,
                                                        FramedSource *inputSource) {
    //printf("UnicastServerMediaSubsession::createNewRTPSink\n");
    return createSink(envir(), rtpGroupsock, rtpPayloadTypeIfDynamic, m_canal);
}

