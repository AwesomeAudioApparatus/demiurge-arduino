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
   _signal.read_fn = adsr_read;
}

Adsr::~Adsr() = default;

void Adsr::configureGate(Signal *gate) {
   _gate = gate;
   _data.gate = &gate->_signal;
}

void Adsr::configureAttack(Signal *control) {
   _attack = control;
   _data.attack = &control->_signal;
}

void Adsr::configureDecay(Signal *control) {
   _decay = control;
   _data.decay = &control->_signal;
}

void Adsr::configureSustain(Signal *control) {
   _sustain = control;
   _data.sustain = &control->_signal;
}

void Adsr::configureRelease(Signal *control) {
   _release = control;
   _data.release = &control->_signal;
}

void Adsr::configureTrig(Signal *trig) {
   _trig = trig;
   _data.trig = &trig->_signal;
}

// TODO: without floating point, this code becomes VERY different, so let's postpone that/
int32_t IRAM_ATTR adsr_read(signal_t *handle, uint64_t time){
   auto *adsr = (adsr_t *)handle->data;
   if( time > adsr->lastCalc) {
      int32_t output;

      bool trigIn = threshold_compute(&adsr->trigThreshold, adsr->trig->read_fn(adsr->trig, time));
      bool gateIn = threshold_compute(&adsr->gateThreshold, adsr->gate->read_fn(adsr->gate, time));

      if (!adsr->currentTrig && trigIn) {
         // RISE
         adsr->stateMachine = 1;
         adsr->startedAt = time;
      }
      adsr->currentTrig = trigIn;

      if (adsr->currentGate) {
         // Ongoing
         switch (adsr->stateMachine) {
            case 1:  // Attack
            {
               int32_t attackIn = adsr->attack->read_fn(adsr->attack, time);
               int32_t k = 20.0 / adsr_slopeTime(attackIn);
               int32_t m = -10.0;
               output = (time - adsr->startedAt) * k + m;
               if (output >= 10.0) {
                  adsr->startedAt = time;
                  adsr->stateMachine++;
               }
               break;
            }
            case 2:  // Decay
            {
               int32_t sustainIn = adsr->sustain->read_fn(adsr->sustain, time);
               int32_t decayIn = adsr->decay->read_fn(adsr->decay, time);
               int32_t k = -(10 - sustainIn) / adsr_slopeTime(decayIn);
               int32_t m = 10;
               output = (time - adsr->startedAt) * k + m;
               if (output >= 10.0) {
                  adsr->startedAt = time;
                  adsr->stateMachine++;
               }
               break;
            }
            case 3:  // Sustain
            {
               output = adsr->sustain->read_fn(adsr->sustain, time);
               break;
            }
            default:
               output = -10;
         }
      } else {
         int32_t releaseIn = adsr->release->read_fn(adsr->release, time);
         int32_t sustainIn = adsr->sustain->read_fn(adsr->sustain, time);
         int32_t k = -sustainIn / adsr_slopeTime(releaseIn);
         int32_t m = sustainIn;
         output = (time - adsr->startedAt) * k + m;
         if (gateIn) {
            // Start new cycle
            adsr->stateMachine = 0;
         }
      }
      adsr->currentGate = gateIn;

      int32_t result = Demiurge::clip(output);
      adsr->cached = result;
      return result;
   }
   return adsr->cached;
}

int32_t IRAM_ATTR adsr_slopeTime(int32_t voltage) {
   // Logarithmic response, so that;
   // -10V = 1 microsecond, 0V = 1 millisecond, +10V = 1 second
   // TODO:
   int32_t millis = pow10(voltage / 3.33333);
   return millis * 1000;
}
