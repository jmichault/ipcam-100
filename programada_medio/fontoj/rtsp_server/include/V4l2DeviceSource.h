/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** V4l2DeviceSource.h
** 
** V4L2 live555 source 
**
** -------------------------------------------------------------------------*/


#ifndef V4L2_DEVICE_SOURCE
#define V4L2_DEVICE_SOURCE

#include <string>
#include <list> 
#include <iostream>
#include <iomanip>

// live555
#include <liveMedia/liveMedia.hh>




// ---------------------------------
// V4L2 FramedSource
// ---------------------------------
class V4L2DeviceSource: public FramedSource
{
	public:
		// ---------------------------------
		// Captured frame
		// ---------------------------------
		struct Frame
		{
			Frame(char* buffer, int size, timeval timestamp) : m_buffer(buffer), m_size(size), m_timestamp(timestamp) {};
			Frame(const Frame&);
			Frame& operator=(const Frame&);
			~Frame()  { delete m_buffer; };
			                    
			char* m_buffer;
			unsigned int m_size;
			timeval m_timestamp;
		};
		// ---------------------------------
		// Compute simple stats
		// ---------------------------------
		class Stats
		{
			public:
				Stats(const std::string & msg) : m_fps(0), m_fps_sec(0), m_size(0), m_msg(msg) {};
				
			public:
				int notify(int tv_sec, int framesize);
			
			protected:
				int m_fps;
				int m_fps_sec;
				int m_size;
				const std::string m_msg;
		};

		
	public:
		static V4L2DeviceSource* createNew(UsageEnvironment& env,  int outputFd, bool useThread) ;
		std::string getAuxLine() { return m_auxLine; };	
		void videocallback( const struct timeval *tv, void *data, size_t len, int keyFrame );

	protected:
		V4L2DeviceSource(UsageEnvironment& env,  int outputFd, bool useThread);
		virtual ~V4L2DeviceSource();

	protected:	

		static void* threadStub(void* clientData) { return ((V4L2DeviceSource*) clientData)->thread();};
		void* thread();
		static void deliverFrameStub(void* clientData) {((V4L2DeviceSource*) clientData)->deliverFrame();};
		void deliverFrame();
		static void incomingPacketHandlerStub(void* clientData, int mask) { ((V4L2DeviceSource*) clientData)->getNextFrame(); };
		int getNextFrame();
		void processFrame(char * frame, int frameSize, const timeval &ref);
		void queueFrame(char * frame, int frameSize, const timeval &tv);


		// split packet in frames
		virtual std::list< std::pair<unsigned char*,size_t> > splitFrames(unsigned char* frame, unsigned frameSize);
		
		// overide FramedSource
		virtual void doGetNextFrame();	
		virtual void doStopGettingFrames();
					
	protected:
		
		Stats m_in;
		Stats m_out;
		EventTriggerId m_eventTriggerId;
		int m_outfd;
		unsigned int m_queueSize;
		pthread_t m_thid;
		pthread_mutex_t m_mutex;
		std::string m_auxLine;
		std::list<Frame*> m_captureQueue;
};

#endif
