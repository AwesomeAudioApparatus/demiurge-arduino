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
#include "Pan.h"


Pan::Pan() {
   _left = new PanChannel(this, 0.05);
   _right = new PanChannel(this, -0.05);
};

void Pan::configure(Signal *input, Signal *control) {
   _input = input;
   _control = control;
}

Signal *Pan::outputLeft() {
   return _left;
}

Signal *Pan::outputRight() {
   return _right;
}

PanChannel::PanChannel(Pan *host, float factor) {
   _host = host;
   _factor = factor;
}

PanChannel::~PanChannel() = default;

float IRAM_ATTR PanChannel::update(uint64_t time) {
   float control = _host->_control->read(time);
   return _host->_input->read(time) * control * _factor;
}
