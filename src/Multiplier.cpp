/*
  Copyright 2020, Awesome Audio Apparatus.

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
#include "Multiplier.h"
#include "Signal.h"

Multiplier::Multiplier() {
   ESP_LOGD("Multiplier", "Constructor: %llx ", (uint64_t) this);
   _signal.data = &_data;
   _signal.read_fn = scale_read;
   ((scale_t *) &_signal)->scale = 1.0;
   _data.scale_control = nullptr;
}

Multiplier::~Multiplier()  = default;

void Multiplier::configure(Signal *input) {
   _data.input = &input->_signal;
   _data.scale_control = nullptr;
}

void Multiplier::configure(Signal *input, Signal *scale_control) {
   _data.input = &input->_signal;
   _data.scale_control = &scale_control->_signal;
}

float Multiplier::get_scale() {
   return _data.scale;
}

void Multiplier::set_scale(float new_scale) {
   _data.scale = new_scale;
}

float IRAM_ATTR scale_read(signal_t *handle, uint64_t time){
   if (time > handle->last_calc) {
      handle->last_calc = time;
      auto *scale = (scale_t *) handle->data;
      float input = scale->input->read_fn(scale->input, time);
      float  new_output;

      if (scale->scale_control != nullptr) {
         float new_scale = handle->read_fn(handle, time);
         new_output = input * new_scale;
      } else {
         new_output = input * scale->scale;
      }
      handle->cached = new_output;
      return new_output;
   }
   return handle->cached;
}
