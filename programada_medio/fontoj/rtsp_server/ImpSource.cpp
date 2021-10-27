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

#include <stdlib.h>
#include <string.h>
#ifdef UZI_DMALLOC
#include <dmalloc.h>
#endif
#include "ImpSource.hh"
#include <groupsock/GroupsockHelper.hh> // for "gettimeofday()"
#include "imp_komuna.h"

char * ImpSource::buffer=NULL;
int ImpSource::bufferSize=0;

ImpSource*
ImpSource::createNew(UsageEnvironment& env,
			int params) {
  return new ImpSource(env, params);
}

EventTriggerId ImpSource::eventTriggerId = 0;

unsigned ImpSource::referenceCount = 0;

ImpSource::ImpSource(UsageEnvironment& env,
			   int params)
  : FramedSource(env), canal(params) {
  printf(" new ImpSource\n");
  if (referenceCount == 0) {
    // Any global initialization of the device would be done here:
    bufferSize = 100000;
    buffer = (char *)malloc( bufferSize);
  }
  ++referenceCount;

  // Any instance-specific initialization of the device would be done here:
  //%%% TO BE WRITTEN %%%

  // We arrange here for our "deliverFrame" member function to be called
  // whenever the next frame of data becomes available from the device.
  //
  // If the device can be accessed as a readable socket, then one easy way to do this is using a call to
  //     envir().taskScheduler().turnOnBackgroundReadHandling( ... )
  // (See examples of this call in the "liveMedia" directory.)
  //
  // If, however, the device *cannot* be accessed as a readable socket, then instead we can implement it using 'event triggers':
  // Create an 'event trigger' for this device (if it hasn't already been done):
  if (eventTriggerId == 0) {
    eventTriggerId = envir().taskScheduler().createEventTrigger(deliverFrame0);
  }
}

ImpSource::~ImpSource() {
  // Any instance-specific 'destruction' (i.e., resetting) of the device would be done here:
  //%%% TO BE WRITTEN %%%
  printf("~ImpSource\n");

  --referenceCount;
  if (referenceCount == 0) {
    // Any global 'destruction' (i.e., resetting) of the device would be done here:
    free(buffer);
    buffer=NULL;
    bufferSize = 0;

    // Reclaim our 'event trigger'
    envir().taskScheduler().deleteEventTrigger(eventTriggerId);
    eventTriggerId = 0;
  }
}

void ImpSource::doGetNextFrame() {
  // This function is called (by our 'downstream' object) when it asks for new data.

  // Note: If, for some reason, the source device stops being readable (e.g., it gets closed), then you do the following:
  if (0 /* the source stops being readable */ /*%%% TO BE WRITTEN %%%*/) {
    handleClosure();
    return;
  }
  int ret = IMP_Encoder_PollingStream(canal, 1000);
  if (ret >= 0)
  {
    IMPEncoderStream stream;
    ret = IMP_Encoder_GetStream(canal, &stream, 1);
    if (ret >= 0)
    {
      int nr_pack = stream.packCount;

      void *memoryAddress = buffer;
      bytesRead = 0;
      for (int i = 0; i < nr_pack; i++)
      {
        int packLen = stream.pack[i].length;
        if ((bytesRead + packLen) > bufferSize )
        {
          bufferSize = (bytesRead + packLen)+10;
          buffer = (char *) realloc(buffer,bufferSize);
          memoryAddress = (void *) ((int) buffer + bytesRead);
        }
        memcpy(memoryAddress, (void *) stream.pack[i].virAddr, packLen);
        memoryAddress = (void *) ((int) memoryAddress + packLen);
        bytesRead = bytesRead + packLen;
      }
      IMP_Encoder_ReleaseStream(canal, &stream);
      if (bytesRead > 0)
      {
        deliverFrame();
        return;
      }
    }
  }

  nextTask() = envir().taskScheduler().scheduleDelayedTask(0,
                                                             (TaskFunc *) FramedSource::afterGetting, this);
}

void ImpSource::deliverFrame0(void* clientData) {
  ((ImpSource*)clientData)->deliverFrame();
}

void ImpSource::deliverFrame() {
  // This function is called when new frame data is available from the device.
  // We deliver this data by copying it to the 'downstream' object, using the following parameters (class members):
  // 'in' parameters (these should *not* be modified by this function):
  //     fTo: The frame data is copied to this address.
  //         (Note that the variable "fTo" is *not* modified.  Instead,
  //          the frame data is copied to the address pointed to by "fTo".)
  //     fMaxSize: This is the maximum number of bytes that can be copied
  //         (If the actual frame is larger than this, then it should
  //          be truncated, and "fNumTruncatedBytes" set accordingly.)
  // 'out' parameters (these are modified by this function):
  //     fFrameSize: Should be set to the delivered frame size (<= fMaxSize).
  //     fNumTruncatedBytes: Should be set iff the delivered frame would have been
  //         bigger than "fMaxSize", in which case it's set to the number of bytes
  //         that have been omitted.
  //     fPresentationTime: Should be set to the frame's presentation time
  //         (seconds, microseconds).  This time must be aligned with 'wall-clock time' - i.e., the time that you would get
  //         by calling "gettimeofday()".
  //     fDurationInMicroseconds: Should be set to the frame's duration, if known.
  //         If, however, the device is a 'live source' (e.g., encoded from a camera or microphone), then we probably don't need
  //         to set this variable, because - in this case - data will never arrive 'early'.
  // Note the code below.

  if (!isCurrentlyAwaitingData()) return; // we're not ready for the data yet

  u_int8_t* newFrameDataStart = (u_int8_t*)buffer; 
  unsigned newFrameSize = bytesRead; 

  // Deliver the data here:
  if (newFrameSize > fMaxSize) {
    fFrameSize = fMaxSize;
    fNumTruncatedBytes = newFrameSize - fMaxSize;
  } else {
    fFrameSize = newFrameSize;
  }
  gettimeofday(&fPresentationTime, NULL); // If you have a more accurate time - e.g., from an encoder - then use that instead.
  // If the device is *not* a 'live source' (e.g., it comes instead from a file or buffer), then set "fDurationInMicroseconds" here.
  memmove(fTo, newFrameDataStart, fFrameSize);

  // After delivering the data, inform the reader that it is now available:
//  FramedSource::afterGetting(this);
    nextTask() = envir().taskScheduler().scheduleDelayedTask(0,
                                                             (TaskFunc *) FramedSource::afterGetting, this);

}


// The following code would be called to signal that a new frame of data has become available.
// This (unlike other "LIVE555 Streaming Media" library code) may be called from a separate thread.
// (Note, however, that "triggerEvent()" cannot be called with the same 'event trigger id' from different threads.
// Also, if you want to have multiple device threads, each one using a different 'event trigger id', then you will need
// to make "eventTriggerId" a non-static member variable of "ImpSource".)
void signalNewFrameData() {
  TaskScheduler* ourScheduler = NULL; //%%% TO BE WRITTEN %%%
  ImpSource* ourDevice  = NULL; //%%% TO BE WRITTEN %%%

  if (ourScheduler != NULL) { // sanity check
    ourScheduler->triggerEvent(ImpSource::eventTriggerId, ourDevice);
  }
}
