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

#include <math.h>
#include "Demiurge.h"

Adsr::Adsr() {
   _gateThreshold = new Threshold();
   _trigThreshold = new Threshold();
}

Adsr::~Adsr() = default;

void Adsr::configureGate(Signal *gate) {
   _gate = gate;
}

void Adsr::configureAttack(Signal *control) {
   _attack = control;
}

void Adsr::configureDecay(Signal *control) {
   _decay = control;

}

void Adsr::configureSustain(Signal *control) {
   _sustain = control;

}

void Adsr::configureRelease(Signal *control) {
   _release = control;
}

void Adsr::configureTrig(Signal *trig) {
   _trig = trig;
}

float IRAM_ATTR Adsr::update(uint64_t time) {
   float output;

   bool trigIn = _trigThreshold->compute(_trig->read(time));
   bool gateIn = _gateThreshold->compute(_gate->read(time));

   if (!_currentTrig && trigIn) {
      // RISE
      stateMachine = 1;
      startedAt = time;
   }
   _currentTrig = trigIn;

   if (_currentGate) {
      // Ongoing
      switch (stateMachine) {
         case 1:  // Attack
         {
            float attackIn = _attack->read(time);
            float k = 20.0 / slopeTime(attackIn);
            float m = -10.0;
            output = (time - startedAt) * k + m;
            if (output >= 10.0) {
               startedAt = time;
               stateMachine++;
            }
            break;
         }
         case 2:  // Decay
         {
            float sustainIn = _sustain->read(time);
            float decayIn = _decay->read(time);
            float k = -(10 - sustainIn) / slopeTime(decayIn);
            float m = 10;
            output = (time - startedAt) * k + m;
            if (output >= 10.0) {
               startedAt = time;
               stateMachine++;
            }
            break;
         }
         case 3:  // Sustain
         {
            output = _sustain->read(time);
            break;
         }
         default:
            output = -10;
      }
   } else {
      float releaseIn = _release->read(time);
      float sustainIn = _sustain->read(time);
      float k = -sustainIn / slopeTime(releaseIn);
      float m = sustainIn;
      output = (time - startedAt) * k + m;
      if (gateIn) {
         // Start new cycle
         stateMachine = 0;
      }
   }
   _currentGate = gateIn;

   return Demiurge::clip(output);
}

float Adsr::slopeTime(float voltage) {
   // Logarithmic response, so that;
   // -10V = 1 microsecond, 0V = 1 millisecond, +10V = 1 second
   // TODO: If log() is too slow, use lookup tables.
   float millis = pow10(voltage / 3.33333);
   return millis * 1000;
}
