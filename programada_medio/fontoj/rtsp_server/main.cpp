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

#include <BasicUsageEnvironment/BasicUsageEnvironment.hh>
#include <GroupsockHelper.hh> // for "weHaveAnIPv*Address()"
#include "DynamicRTSPServer.hh"
#include "imp_komuna.h"


int main(int argc, char** argv) {
  OutPacketBuffer::maxSize = (1920*1080*3/2);
  // Begin by setting up our usage environment:
  TaskScheduler* scheduler = BasicTaskScheduler::createNew();
  UsageEnvironment* env = BasicUsageEnvironment::createNew(*scheduler);

  UserAuthenticationDatabase* authDB = NULL;
#ifdef ACCESS_CONTROL
  // To implement client access control to the RTSP server, do the following:
  authDB = new UserAuthenticationDatabase;
  authDB->addUserRecord("username1", "password1"); // replace these with real strings
  // Repeat the above with each <username>, <password> that you wish to allow
  // access to the server.
#endif
  // ŝanĝas agordojn de kodigilo al mjpg
  channel_attrs[1].encAttr.enType=PT_JPEG;
  channel_attrs[1].encAttr.profile=0;
  // init T21
  imp_init();

  // Create the RTSP server.  Try first with the default port number (554),
  // and then with the alternative port number (8554):
  RTSPServer* rtspServer;
  portNumBits rtspServerPortNum = 554;
  rtspServer = DynamicRTSPServer::createNew(*env, rtspServerPortNum, authDB);
  if (rtspServer == NULL) {
    rtspServerPortNum = 8554;
    rtspServer = DynamicRTSPServer::createNew(*env, rtspServerPortNum, authDB);
  }
  if (rtspServer == NULL) {
    *env << "Failed to create RTSP server: " << env->getResultMsg() << "\n";
    exit(1);
  }

  *env << "T21-hacks RTSP Server\n";

  *env << "Play streams from this server using the URL\n";
  if (weHaveAnIPv4Address(*env)) {
    char* rtspURLPrefix = rtspServer->ipv4rtspURLPrefix();
    *env << "\t" << rtspURLPrefix << "stream1\n";
    *env << "\t" << rtspURLPrefix << "stream2\n";
    delete[] rtspURLPrefix;
    if (weHaveAnIPv6Address(*env)) *env << "or\n";
  }
  if (weHaveAnIPv6Address(*env)) {
    char* rtspURLPrefix = rtspServer->ipv6rtspURLPrefix();
    *env << "\t" << rtspURLPrefix << "stream1\n";
    *env << "\t" << rtspURLPrefix << "stream2\n";
    delete[] rtspURLPrefix;
  }


  // Also, attempt to create a HTTP server for RTSP-over-HTTP tunneling.
  // Try first with the default HTTP port (80), and then with the alternative HTTP
  // port numbers (8000 and 8080).

  if (rtspServer->setUpTunnelingOverHTTP(80) || rtspServer->setUpTunnelingOverHTTP(8000) || rtspServer->setUpTunnelingOverHTTP(8080)) {
    *env << "(We use port " << rtspServer->httpServerPortNum() << " for optional RTSP-over-HTTP tunneling, or for HTTP live streaming (for indexed Transport Stream files only).)\n";
  } else {
    *env << "(RTSP-over-HTTP tunneling is not available.)\n";
  }

  env->taskScheduler().doEventLoop(); // does not return

  return 0;
}
