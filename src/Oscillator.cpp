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
#include <esp_task.h>
#include "Demiurge.h"

static const float SINE_CONST = 1000000 / M_TWOPI;

Oscillator::Oscillator(int mode) {
   _signal.read_fn = volume_read;
   _data.mode = mode;
   _mode = mode;
   _triggerControl = nullptr;
   _amplitudeControl = nullptr;
   _frequencyControl = nullptr;

}

Oscillator::~Oscillator() = default;

void Oscillator::configure(Signal *freqControl, Signal *amplitudeControl, Signal *trigControl) {
   configureFrequency(freqControl);
   configureAmplitude(amplitudeControl);
   configureTrig(trigControl);
}

void Oscillator::configureFrequency(Signal *freqControl) {
   configASSERT(freqControl != nullptr)
   _frequencyControl = freqControl;
   _data.frequency = &freqControl->_signal;
}

void Oscillator::configureAmplitude(Signal *amplitudeControl) {
   configASSERT(amplitudeControl != nullptr)
   _amplitudeControl = amplitudeControl;
   _data.amplitude = &amplitudeControl->_signal;
}

void Oscillator::configureTrig(Signal *trigControl) {
   configASSERT(trigControl != nullptr)
   _triggerControl = trigControl;
   _data.trigger = &trigControl->_signal;
}

float IRAM_ATTR oscillator_read(void *handle, uint64_t time) {
   auto *osc = (oscillator_t *) handle;
   float freq;
   if (osc->frequency == nullptr)
      freq = 440.0f;
   else
      freq = OctavePerVolt::frequencyOf(osc->frequency->read_fn(osc->frequency, time));
   freq = 440.0f;

   float amplitude;
   if (osc->amplitude == nullptr)
      amplitude = 10.0f;
   else
      amplitude = osc->amplitude->read_fn(osc->amplitude, time);

   switch (osc->mode) {
      case DEMIURGE_SINE: {
         Demiurge::runtime().timing[3]->start();
         float result = ((float) isin(freq * (time - osc->lastTrig)/3.2767)) / 4096 * amplitude;
         osc->output = result;
         Demiurge::runtime().timing[3]->stop();
         return result;
      }
      case DEMIURGE_SQUARE: {
         // TODO
         break;
      }
      case DEMIURGE_TRIANGLE: {
         // TODO
         break;
      }
      case DEMIURGE_SAW: {
         // TODO
         break;
      }
   }
}
