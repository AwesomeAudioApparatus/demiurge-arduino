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

#ifndef _DEMIURGE_CALCULATOR_H_
#define _DEMIURGE_CALCULATOR_H_

#include "Signal.h"

typedef struct {
   signal_t *input;
   float (*calc_fn)(float input);
} calculator_t;

float calculator_read(signal_t *handle, uint64_t time);

class Calculator : public Signal {
public:
   Calculator();

   virtual ~Calculator();

   void configure(Signal *input);
   void configure(float (*calc_fn)(float input) );
   void configure(Signal *input, float (*calc_fn)(float input) );

private:
   calculator_t _data{};
};


#endif
