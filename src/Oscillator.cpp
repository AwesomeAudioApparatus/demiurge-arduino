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
#include "sine.h"
#include "Cordic.h"


Oscillator::Oscillator(int mode) {
   _signal.read_fn = oscillator_read;
   _data.mode = mode;
   _triggerControl = nullptr;
   _amplitudeControl = nullptr;
   _frequencyControl = nullptr;
   _signal.data = &_data;
   _data.lastTrig = 0;
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

int32_t IRAM_ATTR oscillator_read(signal_t *handle, uint64_t time_in_us) {
   // time in microseconds
   auto *osc = (oscillator_t *) handle->data;
   if (time_in_us > osc->lastCalc) {
      osc->lastCalc = time_in_us;
      int32_t freq = 20000;
      signal_t *freqControl = osc->frequency;
      if (freqControl != nullptr) {
         freq = octave_frequencyOf(freqControl->read_fn(freqControl, time_in_us));
      }
      int32_t period_in_us = 1000000 / freq;

      float amplitude = 1.0f;
      if (osc->amplitude != nullptr) {
         amplitude = (float) osc->amplitude->read_fn(osc->amplitude, time_in_us) / 4096.0f;
      }

      switch (osc->mode) {
         case DEMIURGE_SINE: {
//            int32_t result = (((float) isin(freq * (time - osc->lastTrig) / 3.2767)) / 4096) * amplitude;

            // time - osc->lastTrig is nanoseconds since last trig.
            double x = freq * (time_in_us - osc->lastTrig);
            double out;
            Cordic::sin_values(osc->n_data, x, out);
            auto result = (int32_t) (out * amplitude);
            osc->cached = result;
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
            uint64_t x = time_in_us % period_in_us;
            double slope = 4095.0 / period_in_us;
            auto out = (int32_t) ((slope * x) * amplitude);
            osc->cached = out;
            return out;
         }
      }
      return 0.0;
   }


   return osc->cached;
}
