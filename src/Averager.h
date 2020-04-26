/*
  Copyright 2020, Awesome Audio Apparatus.

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

#ifndef _DEMIURGE_AVERAGE_H_
#define _DEMIURGE_AVERAGE_H_

#include "Signal.h"

typedef struct {
   float rolling_avg;
   float keep;
   float update;
   signal_t *input;
   signal_t *averaging_control;
} average_t;

float average_read(signal_t *handle, uint64_t time);

class Averager : public Signal {
public:
   Averager();

   virtual ~Averager();

   void configure(Signal *input, Signal *avg_control);

   float get_keep();
   void set_keep(float new_keep);

   float get_update();

   void set_update(float new_update);

private:
   average_t _data{};
};


#endif
