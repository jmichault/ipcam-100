/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** main.cpp
** 
** RTSP using live555                                                                 
**                                                                                    
** -------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sstream>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
// libv4l2
#include <linux/videodev2.h>
// live555
#include <BasicUsageEnvironment/BasicUsageEnvironment.hh>
#include <groupsock/GroupsockHelper.hh>

// project
#include "H264_V4l2DeviceSource.h"
#include "ServerMediaSubsession.h"
#include "ImpJpegVideoDeviceSource.h"
#include "imp_komuna.h"


#define SNX_RTSP_SERVER_VERSION        ("V00.01.04")

// create live555 environment
UsageEnvironment *env = NULL;
TaskScheduler *scheduler = NULL;

// create RTSP server
RTSPServer *rtspServer = NULL;
FramedSource *videoES = NULL;

// -----------------------------------------
//    add an RTSP session
// -----------------------------------------
void addSession(RTSPServer *rtspServer, const char *sessionName, ServerMediaSubsession *subSession,
                ServerMediaSubsession *audio_subSession) {
    UsageEnvironment &env(rtspServer->envir());
    ServerMediaSession *sms = ServerMediaSession::createNew(env, sessionName);
    sms->addSubsession(subSession);

    if (audio_subSession)
        sms->addSubsession(audio_subSession);

    rtspServer->addServerMediaSession(sms);

    char *url = rtspServer->rtspURL(sms);

    fprintf(stderr, "lay this stream using the URL: \"%s\"\n", url);

    delete[] url;
}




// -----------------------------------------
//    signal handler
// -----------------------------------------
/*
 * NOTE: Please finish this program by kill -2
 */

char quit = 0;

void sighandler(int n) {
    printf("Signal received (%d)\n", n);
    quit = 1;
#if 0

    if(videoES)
        Medium::close(videoES);




    if (rtspServer)
    Medium::close(rtspServer);

    if (env)
        env->reclaim();
    delete scheduler;
#endif
}


// -----------------------------------------
//    entry point
// -----------------------------------------
int main(int argc, char **argv) {

    // default parameters
    const char *dev_name = "/dev/video1";
    int format = V4L2_PIX_FMT_MJPEG;
    int width = 1280;
    int height = 720;
    int queueSize = 5;
    int fps = 5;
    int isp_fps = 5;
    int bitrate = 1024; //(Kbps)
    int mjpeg_qp = 60;
    int m2m_en = 1;
    int gop = fps;
    unsigned short rtpPortNum = 20000;
    unsigned short rtcpPortNum = rtpPortNum + 1;
    unsigned char ttl = 5;
    struct in_addr destinationAddress;
    unsigned short rtspPort = 554;
    unsigned short rtspOverHTTPPort = 0;
    bool multicast = false;
    int verbose = 0;
    std::string outputFile;
    //bool useMmap = true;
    std::string url = "media/stream1";
    std::string murl = "multicast";
    bool useThread = true;
    in_addr_t maddr = INADDR_NONE;


    // decode parameters
    int c = 0;
    //while ((c = getopt (argc, argv, "hW:H:Q:P:F:v::O:T:m:u:rsM:")) != -1)
    while ((c = getopt(argc, argv, "hb:W:H:g:Q:P:F:i:O:T:m:u:M:j:")) != -1)
    {
        switch (c) {
            case 'O':
                outputFile = optarg;
                break;
            case 'v':
                verbose = 1;
                if (optarg && *optarg == 'v') verbose++;
                break;
            case 'm':
                multicast = true;
                if (optarg) murl = optarg;
                break;
            case 'M':
                multicast = true;
                if (optarg) maddr = inet_addr(optarg);
                break;
            case 'g':
                gop = atoi(optarg);
                break;
            case 'b':
                bitrate = atoi(optarg);
                break;
            case 'W':
                width = atoi(optarg);
                break;
            case 'H':
                height = atoi(optarg);
                break;
            case 'Q':
                queueSize = atoi(optarg);
                break;
            case 'P':
                rtspPort = atoi(optarg);
                break;
            case 'T':
                rtspOverHTTPPort = atoi(optarg);
                break;
            case 'F':
                fps = atoi(optarg);
                break;
            case 'i':
                isp_fps = atoi(optarg);
                break;
                //case 'r':	useMmap =  false; break;
                //case 's':	useThread =  false; break;
            case 'u':
                url = optarg;
                break;
            case 'j':
                format = V4L2_PIX_FMT_MJPEG;
                mjpeg_qp = atoi(optarg);
                break;
            case 'h':
            default: {
                std::cout << argv[0] << "Version:" << SNX_RTSP_SERVER_VERSION << std::endl;
                std::cout << "Usage :" << std::endl;
                std::cout << "\t " << argv[0]
                          << " [-a] [-j mjpeg_qp] [-m] [-P RTSP port][-T RTSP/HTTP port][-Q queueSize] [-M groupaddress] [-b bitrate] [-W width] [-H height] [-F fps] [-i isp_fps] [device]"
                          << std::endl;

                std::cout << "\t -Q length: Number of frame queue  (default " << queueSize << ")" << std::endl;
                std::cout << "\t RTSP options :" << std::endl;
                std::cout << "\t -u url     : unicast url (default " << url << ")" << std::endl;
                std::cout << "\t -m url     : multicast url (default " << murl << ")" << std::endl;
                std::cout << "\t -M addr    : multicast group   (default is a random address)" << std::endl;
                std::cout << "\t -P port    : RTSP port (default " << rtspPort << ")" << std::endl;
                std::cout << "\t -T port    : RTSP over HTTP port (default " << rtspOverHTTPPort << ")" << std::endl;
                std::cout << "\t V4L2 options :" << std::endl;
                //std::cout << "\t -r       : V4L2 capture using read interface (default use memory mapped buffers)" << std::endl;
                //std::cout << "\t -s       : V4L2 capture using live555 mainloop (default use a separated reading thread)" << std::endl;
                std::cout << "\t -F fps     : V4L2 capture framerate (default " << fps << ")" << std::endl;
                std::cout << "\t -i isp_fps : ISP capture framerate (default " << isp_fps << ")" << std::endl;
                std::cout << "\t -W width   : V4L2 capture width (default " << width << ")" << std::endl;
                std::cout << "\t -H height  : V4L2 capture height (default " << height << ")" << std::endl;

                std::cout << "\t V4L2 H264 options :" << std::endl;

                std::cout << "\t -b bitrate : V4L2 capture bitrate kbps(default " << bitrate << " kbps)" << std::endl;
                std::cout << "\t -g gop     : V4L2 capture gop (default " << gop << " )" << std::endl;
                std::cout << "\t device     : V4L2 capture device (default " << dev_name << ")" << std::endl;

                std::cout << "\t V4L2 MJPEG options :" << std::endl;
                std::cout << "\t -j mjpeg_qp : MJPEG streaming and qp (default is 60)" << std::endl;

                std::cout << "\t H264 example : " << argv[0] << " -Q 5 -u media/stream1 -P 554" << std::endl;
                std::cout << "\t MJPEG example : " << argv[0] << " -W 640 -H 480 -j 120 -Q 5 -u media/stream1 -P 554"
                          << std::endl;
                exit(0);
            }
        }
    }
    if (optind < argc) {
        dev_name = argv[optind];
    }
  // ŝanĝas agordojn de kodigilo al mjpg
  for ( int i = 0; i <= 1; i++)
  {
    channel_attrs[i].encAttr.enType=PT_JPEG;
    channel_attrs[i].encAttr.profile=0;
  }

    // pravalorizo de T21
    imp_init();

    // create live555 environment
    scheduler = BasicTaskScheduler::createNew();
    env = BasicUsageEnvironment::createNew(*scheduler);

    // create RTSP server
    rtspServer = RTSPServer::createNew(*env, rtspPort);
    if (rtspServer == NULL) {
        //LOG(ERROR) << "Failed to create RTSP server: " << env->getResultMsg();
        fprintf(stderr, "Failed to create RTSP server: %s \n", env->getResultMsg());
    } else {
        // set http tunneling
        if (rtspOverHTTPPort) {
            rtspServer->setUpTunnelingOverHTTP(rtspOverHTTPPort);
        }

        // Init capture
        //LOG(NOTICE) << "Create V4L2 Source..." << dev_name;
        fprintf(stderr, "create Video source = %s \n", dev_name);



        /*  check if create a Device source success */
        if (videoES != NULL) {
            //LOG(FATAL) << "Unable to create source for device " << dev_name;
            fprintf(stderr, "Unable to create source for device  %s \n", dev_name);
        } else {

            // Setup the outpacket size;
            if (m2m_en) {
                //OutPacketBuffer::maxSize = (unsigned int)videoCapture->m2m->isp_buffers->length;
                OutPacketBuffer::maxSize = bitrate << 8;    //2X Bitrate as the max packet size
            } else {

                OutPacketBuffer::maxSize = width * height * 3 / 2;
            }

            ServerMediaSubsession *video_subSession = NULL;
            ServerMediaSubsession *audio_subSession = NULL;

            video_subSession = UnicastServerMediaSubsession::createNew(*env, format);
fprintf(stderr,"session créée.\n");

            // Create Server Unicast Session
            addSession(rtspServer, url.c_str(), video_subSession, audio_subSession);

            // main loop
            signal(SIGINT, sighandler);
            env->taskScheduler().doEventLoop(&quit);

            fprintf(stderr, "Exiting....  \n");

            Medium::close(videoES);
        }

    }
    Medium::close(rtspServer);

    imp_exit();

    env->reclaim();
    delete scheduler;

    return 0;
}



