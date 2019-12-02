/*
  Copyright 2019, Awesome Audio Apparatus.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

      https://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
      limitations under the License.
*/

#ifndef _DEMIURGE_TIMING_H_
#define _DEMIURGE_TIMING_H_

#include <stdint.h>

class Timing {

public:
   Timing(const char *name);
   bool started();
   bool start();
   void stop();
   uint64_t lastInterval();
   void report();

private:
   const char *_name;
   bool _started;
   uint64_t _startedAt;
   uint64_t _stoppedAt;

   uint64_t _lastInterval;
   int _overruns;
};


#endif
