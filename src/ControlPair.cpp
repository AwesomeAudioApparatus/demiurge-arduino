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
   configASSERT(position > 0 && position <= 8 )
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

float IRAM_ATTR ControlPair::update(uint64_t time) {
   float potIn = _potentiometer->read(time) * _potentiometerScale;
   float cvIn = _cvIn->read(time) * _cvScale;
   return potIn + cvIn;
}
