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

protected:
   double update(uint64_t time) override;

private:
   Signal *_gate;
   Signal *_trig;
   Signal *_attack;
   Signal *_decay;
   Signal *_sustain;
   Signal *_release;

   Threshold *_gateThreshold;
   Threshold *_trigThreshold;

   int stateMachine = 0;
   double doneAt = 0.0;
   double startedAt = 0.0;
   bool _currentTrig;
   bool _currentGate;

   double slopeTime(double voltage);
};


#endif
