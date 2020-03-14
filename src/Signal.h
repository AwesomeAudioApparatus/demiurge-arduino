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

// All signals uses float for representation.
// Audio = -10.0 to 10.0
// CV = -5.0 to 5.0
// Gate = 0V or >=1V
typedef struct signal_t signal_t;

typedef float (*signal_fn)(signal_t *, uint64_t);

typedef struct signal_t {
   void *data;
   float extra1;
   float extra2;
   float extra3;
   float extra4;
   signal_fn read_fn;
   uint64_t last_calc = 0;
   float cached;
} signal_t;

class Signal {

public:
   Signal() noexcept;

   virtual ~Signal();

   signal_t _signal{};

   float extra1();

   float extra2();

   float extra3();

   float extra4();
};

#endif
