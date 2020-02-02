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

#include "Inverter.h"
#include "esp_system.h"


// TODO:
Inverter::Inverter(Signal *midpoint, Signal *scale) {
   _data.midpoint = &midpoint->_signal;
   _data.scale = &scale->_signal;
   _signal.data = &_data;
}

Inverter::~Inverter() = default;

void Inverter::configure(Signal *input) {
   _data.input = &input->_signal;
}

float IRAM_ATTR inverter_read(signal_t *handle, uint64_t time)
{
   auto *inverter = (inverter_t *) handle->data;
   if( time > handle->last_calc ) {
      handle->last_calc = time;
      float out = -handle->read_fn(inverter->input, time);
      handle->cached = out;
      return out;
   }
   return handle->cached;
}
