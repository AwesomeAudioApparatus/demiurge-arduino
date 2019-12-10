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

#include "Demiurge.h"

ControlPair::ControlPair(int position) {
   configASSERT(position > 0 && position <= 8)
   _potentiometer = new Potentiometer(position);
   _cvIn = new CvInPort(position);
   _potentiometerScale = 1.0;
   _cvScale = 1.0;
}

ControlPair::~ControlPair() {
   delete _potentiometer;
   delete _cvIn;
}

void ControlPair::setPotentiometerScale(float scale) {
   _potentiometerScale = scale;
}

void ControlPair::setCvScale(float scale) {
   _cvScale = scale;
}

float IRAM_ATTR controlpair_read(void *handle, uint64_t time) {
   auto *control = (control_pair_t *) handle;
   float potIn = control->potentiometer->read_fn(control->potentiometer, time);
   float cvIn = control->cv->read_fn(control->cv, time);
   return potIn + cvIn;
}
