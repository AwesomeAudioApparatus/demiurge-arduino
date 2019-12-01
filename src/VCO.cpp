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
#include "VCO.h"
#include "OctavePerVolt.h"

static const double SINE_CONST = 1000000 / M_TWOPI;

VCO::VCO(int mode) {
   _mode = mode;
   _triggerControl = nullptr;
   _amplitudeControl = nullptr;
   _frequencyControl = nullptr;
}

VCO::~VCO() = default;

void VCO::configureFrequency(Signal *freqControl) {
   configASSERT(freqControl != nullptr)
   _frequencyControl = freqControl;
}

void VCO::configureAmplitude(Signal *amplitudeControl) {
   configASSERT(amplitudeControl != nullptr)
   _amplitudeControl = amplitudeControl;
}

void VCO::configureTrig(Signal *trigControl) {
   configASSERT(trigControl != nullptr)
   _triggerControl = trigControl;
}

double VCO::update(double time) {
   double freq;
   if (_frequencyControl == nullptr)
      freq = 440;
   else
      freq = OctavePerVolt::frequencyOf(_frequencyControl->read(time));

   double amplitude;
   if (_amplitudeControl == nullptr)
      amplitude = 1.0;
   else
      amplitude = _amplitudeControl->read(time);

   switch (_mode) {
      case DEMIURGE_SINE:
         return sin(freq * (time - _lastTrig) / SINE_CONST) * amplitude;
      case DEMIURGE_SQUARE:
         // TODO
         break;
      case DEMIURGE_TRIANGLE:
         // TODO
         break;
      case DEMIURGE_SAW:
         // TODO
         break;
   }
}
