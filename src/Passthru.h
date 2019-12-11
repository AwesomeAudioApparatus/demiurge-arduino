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

#ifndef _DEMIURGE_PASSTHRU_H
#define _DEMIURGE_PASSTHRU_H


#include "Signal.h"

typedef struct {
   signal_t *me;
   signal_t *input;
   uint64_t lastCalc;
   float cached;
} passthru_t;

float passthru_read(signal_t *handle, uint64_t time);

class Passthru : public Signal {

public:
   Passthru();

   ~Passthru() override;

   void configure(Signal *input);

private:
   passthru_t _data;

   Signal *_input;
};


#endif
