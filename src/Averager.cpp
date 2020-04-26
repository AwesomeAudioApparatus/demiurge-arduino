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
#include "Averager.h"
#include "Signal.h"


Averager::Averager() {
   ESP_LOGD("Averager", "Constructor: %llx ", (uint64_t) this);
   _signal.data = &_data;
   _signal.read_fn = average_read;
   ((average_t *) &_signal)->update = 0.1;
   ((average_t *) &_signal)->keep = 0.9;
   _data.averaging_control = nullptr;
}

Averager::~Averager()  = default;

void Averager::configure(Signal *input, Signal *avg_control) {
   _data.input = &input->_signal;
   _data.averaging_control = &avg_control->_signal;
}

float Averager::get_keep() {
   return _data.keep;
}

void Averager::set_keep(float new_keep) {
   _data.keep = new_keep;
   _data.update = 1 - new_keep;
}

float Averager::get_update() {
   return _data.update;
}

void Averager::set_update(float new_update) {
   _data.keep = 1 - new_update;
   _data.update = new_update;
}

float IRAM_ATTR average_read(signal_t *handle, uint64_t time){
   if (time > handle->last_calc) {
      handle->last_calc = time;
      auto *average = (average_t *) handle->data;
      float input = average->input->read_fn(average->input, time);

      float old_avg = average->rolling_avg;
      float  new_output;

      if (average->averaging_control != nullptr) {
         float keep = handle->read_fn(handle, time) / 20 + 10;
         new_output = old_avg * keep + input * (1-keep);
      } else {
         new_output = old_avg * average->keep + old_avg * average->update;
      }
      average->rolling_avg = new_output;
      handle->cached = new_output;
      return new_output;
   }
   return handle->cached;
}
