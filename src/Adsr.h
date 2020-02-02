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

#ifndef _DEMIURGE_ADSR_H_
#define _DEMIURGE_ADSR_H_

#include "Signal.h"
#include "Threshold.h"

typedef struct {
   signal_t *me;
   signal_t *attack;
   signal_t *decay;
   signal_t *sustain;
   signal_t *release;
   signal_t *gate;
   signal_t *trig;
   int stateMachine = 0;
   int64_t doneAt = 0.0;
   int64_t startedAt = 0.0;
   bool currentTrig;
   bool currentGate;
   threshold_t gateThreshold;
   threshold_t trigThreshold;
} adsr_t;

float adsr_read(signal_t *handle, uint64_t time);
float adsr_slopeTime(float voltage);

class Adsr : public Signal {

public:
   Adsr();

   ~Adsr() override;

   void configureGate(Signal *gate);

   void configureAttack(Signal *control);

   void configureDecay(Signal *control);

   void configureSustain(Signal *control);

   void configureRelease(Signal *control);

   void configureTrig(Signal *trig);

private:
   adsr_t _data{};
   Signal *_gate;
   Signal *_trig;
   Signal *_attack;
   Signal *_decay;
   Signal *_sustain;
   Signal *_release;
};


#endif
