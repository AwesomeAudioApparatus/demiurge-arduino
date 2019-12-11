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

#ifndef _DEMIURGE_INVERTER_H_
#define _DEMIURGE_INVERTER_H_

#include "Signal.h"

typedef struct {
   signal_t *me;
   signal_t *input;
   signal_t *scale;
   signal_t *midpoint;
   uint64_t lastCalc;
   float cached;
} inverter_t;

float inverter_read(signal_t *handle, uint64_t time);

class Inverter : public Signal {

public:
   Inverter(Signal *midpoint, Signal *scale);
   ~Inverter() override;

   void configure(Signal *input);

private:
   inverter_t _data;
   Signal* _input{};
};


#endif
