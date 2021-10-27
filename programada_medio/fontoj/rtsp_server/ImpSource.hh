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

#ifndef _IMP_SOURCE_HH
#define _IMP_SOURCE_HH

#ifndef _FRAMED_SOURCE_HH
#include "liveMedia/FramedSource.hh"
#endif


class ImpSource: public FramedSource {
public:
  static ImpSource* createNew(UsageEnvironment& env,
				 int canal );

public:
  static EventTriggerId eventTriggerId;
  // Note that this is defined here to be a static class variable, because this code is intended to illustrate how to
  // encapsulate a *single* device - not a set of devices.
  // You can, however, redefine this to be a non-static member variable.

protected:
  ImpSource(UsageEnvironment& env, int canal);
  // called only by createNew(), or by subclass constructors
  virtual ~ImpSource();

private:
  // redefined virtual functions:
  virtual void doGetNextFrame();
  //virtual void doStopGettingFrames(); // optional

private:
  static void deliverFrame0(void* clientData);
  void deliverFrame();

private:
  static unsigned referenceCount; // used to count how many instances of this class currently exist
  int canal;
  static char * buffer;
  static int bufferSize;
  int bytesRead=0;
};

#endif
