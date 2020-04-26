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
#include <math.h>
#include "Log2.h"
#include "Signal.h"

Log2::Log2() {
   ESP_LOGD("Log2", "Constructor: %llx ", (uint64_t) this);
   _signal.data = &_data;
   _signal.read_fn = log2_read;
}

Log2::~Log2()  = default;

void Log2::configure(Signal *input) {
   _data.input = &input->_signal;
}

float IRAM_ATTR log2_read(signal_t *handle, uint64_t time){
   if (time > handle->last_calc) {
      handle->last_calc = time;
      auto *log2 = (log2_t *) handle->data;
      float input = log2->input->read_fn(log2->input, time);
      float  new_output = log2f(input);
      handle->cached = new_output;
      return new_output;
   }
   return handle->cached;
}
