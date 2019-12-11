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

typedef struct signal_t signal_t;

typedef float (*signal_fn)(signal_t *, uint64_t);

typedef struct signal_t {
   void *data;
   float scale;
   float offset;
   signal_fn read_fn;
   bool noRecalc;
} signal_t;

class Signal  {

public:
   Signal() noexcept;

   virtual ~Signal() = 0;

   float scale();

   float offset();

   void setScale(float scale);

   void setOffset(float offset);

   signal_t _signal{};

private:
   float _lastRead = -1.0;
   float _output = 0.0;
   float _scale = 1.0;
   float _offset = 0.0;
   bool _noRecalc = true;
};

#endif
