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

#include "Passthru.h"
#include <esp_system.h>

Passthru::Passthru() {
   _signal.read_fn = passthru_read;
}

Passthru::~Passthru() = default;

void Passthru::configure(Signal *input) {
   _input = input;
   _data.input = &input->_signal;
   _signal.data = &_data;
}

float IRAM_ATTR passthru_read(signal_t *handle, uint64_t time) {
   auto *passthru = (passthru_t *) handle->data;
   if (time > passthru->lastCalc) {
      passthru->lastCalc = time;
      float input = passthru->input->read_fn(passthru->input, time);
      signal_t *data = passthru->me;
      float result;
      if (data->noRecalc) {
         result = input;
      } else {
         result = input * data->scale + data->offset;
      }
      passthru->cached = result;
      return result;
   }
   return passthru->cached;
}
