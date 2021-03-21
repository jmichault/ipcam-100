/* 
 * Copyright (c) 2021 Jean Michault.
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "DynamicRTSPServer.hh"
#include <liveMedia/liveMedia.hh>
#include <string.h>
#include "H264ImpServerMediaSubsession.hh"
#include "ServerMediaSubsession.h"
#include "imp_komuna.h"

DynamicRTSPServer*
DynamicRTSPServer::createNew(UsageEnvironment& env, Port ourPort,
			     UserAuthenticationDatabase* authDatabase,
			     unsigned reclamationTestSeconds) {
  int ourSocketIPv4 = setUpOurSocket(env, ourPort, AF_INET);
  int ourSocketIPv6 = setUpOurSocket(env, ourPort, AF_INET6);
  if (ourSocketIPv4 < 0 && ourSocketIPv6 < 0) return NULL;

  return new DynamicRTSPServer(env, ourSocketIPv4, ourSocketIPv6, ourPort,
			       authDatabase, reclamationTestSeconds);

}

DynamicRTSPServer::DynamicRTSPServer(UsageEnvironment& env, int ourSocketIPv4, int ourSocketIPv6,
				     Port ourPort,
				     UserAuthenticationDatabase* authDatabase, unsigned reclamationTestSeconds)
  : RTSPServer(env, ourSocketIPv4, ourSocketIPv6, ourPort, authDatabase, reclamationTestSeconds) {
}

DynamicRTSPServer::~DynamicRTSPServer() {
}

static ServerMediaSession* createNewSMS(UsageEnvironment& env,
					char const* fileName, FILE* fid); // forward

void DynamicRTSPServer
::lookupServerMediaSession(char const* streamName,
	lookupServerMediaSessionCompletionFunc* completionFunc,
	void* completionClientData,
	 Boolean isFirstLookupInSession)
{

  // check whether we already have a "ServerMediaSession" for this name:
  ServerMediaSession* sms = getServerMediaSession(streamName);

    if (sms == NULL) {
      sms = createNewSMS(envir(), streamName, NULL); 
      addServerMediaSession(sms);
    }
  if (completionFunc != NULL) {
    (*completionFunc)(completionClientData, sms);
  }
}


#define NEW_SMS(description) do {\
char const* descStr = description\
    ", streamed by T21-hacks";\
sms = ServerMediaSession::createNew(env, fileName, fileName, descStr);\
} while(0)

static ServerMediaSession* createNewSMS(UsageEnvironment& env,
					char const* fileName, FILE* /*fid*/) {
  char const* extension = strrchr(fileName, '.');
  ServerMediaSession* sms = NULL;
  Boolean const reuseSource = True;
  if (strcmp(fileName, "stream1") == 0) {
    NEW_SMS("stream1");
    if (channel_attrs[0].encAttr.enType==PT_JPEG)
      sms->addSubsession(UnicastServerMediaSubsession::createNew(env, 0));
    else
      sms->addSubsession(H264ImpServerMediaSubsession::createNew(env, fileName, reuseSource));
  } else if (strcmp(fileName, "stream2") == 0) {
    NEW_SMS("stream2");
    if (channel_attrs[1].encAttr.enType==PT_JPEG)
      sms->addSubsession(UnicastServerMediaSubsession::createNew(env, 1));
    else
      sms->addSubsession(H264ImpServerMediaSubsession::createNew(env, fileName, reuseSource));
  } else if (strcmp(extension, ".264") == 0) {
    // Assumed to be a H.264 Video Elementary Stream file:
    NEW_SMS("H.264 Video");
    OutPacketBuffer::maxSize = 100000; // allow for some possibly large H.264 frames
    sms->addSubsession(H264VideoFileServerMediaSubsession::createNew(env, fileName, reuseSource));
  }

  return sms;
}
