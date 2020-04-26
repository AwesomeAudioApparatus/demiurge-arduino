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

#ifndef _DEMIURGE_OFFSET_H_
#define _DEMIURGE_OFFSET_H_

#include "Signal.h"

typedef struct {
   float offset;
   signal_t *input;
   signal_t *offset_control;
} offset_t;

float offset_read(signal_t *handle, uint64_t time);

class Adder : public Signal {
public:
   Adder();

   virtual ~Adder();

   void configure(Signal *input);

   void configure(Signal *input, Signal *offset_control);

   float get_offset();
   void set_offset(float new_offset);

private:
   offset_t _data{};
};


#endif
