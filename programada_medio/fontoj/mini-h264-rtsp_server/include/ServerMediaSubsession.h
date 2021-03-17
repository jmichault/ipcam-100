/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** ServerMediaSubsession.h
** 
** -------------------------------------------------------------------------*/

#ifndef SERVER_MEDIA_SUBSESSION
#define SERVER_MEDIA_SUBSESSION

#include <string>
#include <iomanip>

// live555
#include <liveMedia/liveMedia.hh>
// forward declaration

// ---------------------------------
//   BaseServerMediaSubsession
// ---------------------------------
class BaseServerMediaSubsession
{
	public:
		BaseServerMediaSubsession(StreamReplicator* replicator): m_replicator(replicator) {};
	
	public:
		static FramedSource* createSource(UsageEnvironment& env, FramedSource * videoES, int format);
		static RTPSink* createSink(UsageEnvironment& env, Groupsock * rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, int format);
		
	protected:
		StreamReplicator* m_replicator;

};

// -----------------------------------------
//    ServerMediaSubsession for Unicast
// -----------------------------------------
class UnicastServerMediaSubsession : public OnDemandServerMediaSubsession , public BaseServerMediaSubsession
{
	public:
		static UnicastServerMediaSubsession* createNew(UsageEnvironment& env, StreamReplicator* replicator, int format);
		
	protected:
		UnicastServerMediaSubsession(UsageEnvironment& env, StreamReplicator* replicator, int format)
				: OnDemandServerMediaSubsession(env, False), BaseServerMediaSubsession(replicator), m_format(format) {};

		virtual FramedSource* createNewStreamSource(unsigned clientSessionId, unsigned& estBitrate);
		virtual RTPSink* createNewRTPSink(Groupsock* rtpGroupsock,  unsigned char rtpPayloadTypeIfDynamic, FramedSource* inputSource);		
					
	protected:
		int m_format;

};

#endif
