/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** H264_V4l2DeviceSource.cpp
** 
** H264 V4L2 Live555 source 
**
** -------------------------------------------------------------------------*/

#include <sstream>
#include <utility>

// live555
#include <liveMedia/Base64.hh>

// project
#include "H264_V4l2DeviceSource.h"

// ---------------------------------
// H264 V4L2 FramedSource
// ---------------------------------
H264_V4L2DeviceSource* H264_V4L2DeviceSource::createNew(UsageEnvironment& env, int outputFd, bool useThread)
{ 	
	H264_V4L2DeviceSource* source = NULL;

	source = new H264_V4L2DeviceSource(env, outputFd, useThread);
	return source;
}

// Constructor
H264_V4L2DeviceSource::H264_V4L2DeviceSource(UsageEnvironment& env, int outputFd, bool useThread)
	: V4L2DeviceSource(env, outputFd,useThread)
{
}

// Destructor
H264_V4L2DeviceSource::~H264_V4L2DeviceSource()
{	
}

// split packet in frames					
std::list< std::pair<unsigned char*,size_t> > H264_V4L2DeviceSource::splitFrames(unsigned char* frame, unsigned frameSize) 
{				
	std::list< std::pair<unsigned char*,size_t> > frameList;
	
	size_t bufSize = frameSize;
	size_t size = 0;
	unsigned char* buffer = this->extractFrame(frame, bufSize, size);
	while (buffer != NULL)				
	{
		//frameList.push_back(std::make_pair<unsigned char*,size_t>(buffer, size));
		frameList.push_back(std::make_pair(buffer, size));
		switch (buffer[0]&0x1F)					
		{
			case 7: 
				//LOG(INFO) << "SPS";
				//fprintf(stderr, "SPS\n" );
				m_sps.assign((char*)buffer,size);
				break;
			case 8: 
				//fprintf(stderr, "PPS\n" );
				m_pps.assign((char*)buffer,size);
				break;
			default: break;
		}
		
		if (m_auxLine.empty() && !m_sps.empty() && !m_pps.empty())
		{
			u_int32_t profile_level_id = 0;					
			if (m_sps.size() >= 4) profile_level_id = (m_sps[1]<<16)|(m_sps[2]<<8)|m_sps[3]; 
		
			char* sps_base64 = base64Encode(m_sps.c_str(), m_sps.size());
			char* pps_base64 = base64Encode(m_pps.c_str(), m_pps.size());		

			std::ostringstream os; 
			os << "profile-level-id=" << std::hex << std::setw(6) << profile_level_id;
			os << ";sprop-parameter-sets=" << sps_base64 <<"," << pps_base64;
			m_auxLine.assign(os.str());
			
			free(sps_base64);
			free(pps_base64);
			//LOG(NOTICE) << m_auxLine;
			fprintf(stderr, "m_a %s\n", m_auxLine.c_str() );
		}
		
		buffer = this->extractFrame(&buffer[size], bufSize, size);
	}
	return frameList;
}

// extract a frame
unsigned char*  H264_V4L2DeviceSource::extractFrame(unsigned char* frame, size_t& size, size_t& outsize)
{			
	unsigned char * outFrame = NULL;
	outsize = 0;
	if ( (size>= sizeof(H264marker)) && (memcmp(frame,H264marker,sizeof(H264marker)) == 0) )
	{
		size -=  sizeof(H264marker);
		outFrame = &frame[sizeof(H264marker)];
		outsize = size;
		for (int i=0; i+sizeof(H264marker) < size; ++i)
		{
			if (memcmp(&outFrame[i],H264marker,sizeof(H264marker)) == 0)
			{
				outsize = i;
				break;
			}
		}
		size -=  outsize;
	}
	return outFrame;
}

