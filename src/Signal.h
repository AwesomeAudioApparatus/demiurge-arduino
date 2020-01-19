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

#ifndef _DEMIURGE_SIGNAL_H_
#define _DEMIURGE_SIGNAL_H_


#include <stdint.h>

// All signals uses int32_t for representation, where the middle 16 bits represent the audio,
// and the 4 LSB of those are clipped in the 12 bit DAC.
//
// For all scaling operations,
//    positive scale ==> amplify signal by (scale >> 16 )
//    negative scale ==> attenuate signal by (-scale >> 16 )
typedef struct signal_t signal_t;

typedef int32_t (*signal_fn)(signal_t *, uint64_t);

int32_t scale_output(int32_t value, int32_t scale, int32_t offset);

typedef struct signal_t {
   void *data;
   int32_t scale;
   int32_t offset;
   signal_fn read_fn;
   bool noRecalc;
} signal_t;

class Signal  {

public:
   Signal() noexcept;

   virtual ~Signal() = 0;

   int32_t  scale();

   int32_t  offset();

   void setScale(int32_t  scale);

   void setOffset(int32_t  offset);

   signal_t _signal{};

private:
   int32_t  _scale = 0;
   int32_t  _offset = 0;
   int32_t  _noRecalc = true;
};

#endif
