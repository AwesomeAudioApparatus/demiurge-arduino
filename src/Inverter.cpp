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

#include <esp_system.h>
#include <esp_log.h>
#include "Inverter.h"


// TODO:
Inverter::Inverter() {
   ESP_LOGD("Inverter", "Constructor: %llx", (uint64_t) this);
   _signal.data = &_data;
   _data.midpoint = nullptr;
   _data.scale = nullptr;
}

Inverter::~Inverter() = default;

void Inverter::configureMidPoint(Signal *midpoint) {
   _data.midpoint = &midpoint->_signal;
}

void Inverter::configureScale(Signal *scale) {
   _data.scale = &scale->_signal;
}

void Inverter::configure(Signal *input) {
   _data.input = &input->_signal;
}

float IRAM_ATTR inverter_read(signal_t *handle, uint64_t time) {
   auto *inverter = (inverter_t *) handle->data;
   if (time > handle->last_calc) {
      handle->last_calc = time;
      float out;
      signal_t *midpointSignal = inverter->midpoint;
      signal_t *scaleSignal = inverter->scale;
      if (midpointSignal == nullptr) {
         out = 0 - handle->read_fn(inverter->input, time);
      } else {
         float midpoint = midpointSignal->read_fn(midpointSignal, time);
         out = midpoint - handle->read_fn(inverter->input, time);
      }
      if (scaleSignal != nullptr) {
         out = out * scaleSignal->read_fn(scaleSignal, time);
      }
      handle->cached = out;
      return out;
   }
   return handle->cached;
}
