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
#include "Adder.h"
#include "Signal.h"


Adder::Adder() {
   ESP_LOGD("Adder", "Constructor: %llx ", (uint64_t) this);
   _signal.data = &_data;
   _signal.read_fn = offset_read;
   ((offset_t *) &_signal)->offset = 0;
   _data.offset_control = nullptr;
}

Adder::~Adder()  = default;

void Adder::configure(Signal *input) {
   _data.input = &input->_signal;
   _data.offset_control = nullptr;
}

void Adder::configure(Signal *input, Signal *offset_control) {
   _data.input = &input->_signal;
   _data.offset_control = &offset_control->_signal;
}

float Adder::get_offset() {
   return _data.offset;
}

void Adder::set_offset(float new_offset) {
   _data.offset = new_offset;
}

float IRAM_ATTR offset_read(signal_t *handle, uint64_t time){
   if (time > handle->last_calc) {
      handle->last_calc = time;
      auto *offset = (offset_t *) handle->data;
      float input = offset->input->read_fn(offset->input, time);
      float  new_output;

      if (offset->offset_control != nullptr) {
         float new_offset = handle->read_fn(handle, time);
         new_output = input + new_offset;
      } else {
         new_output = input + offset->offset;
      }
      handle->cached = new_output;
      return new_output;
   }
   return handle->cached;
}
