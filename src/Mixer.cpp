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

#include <malloc.h>
#include <esp_system.h>
#include <esp_task.h>
#include <esp_log.h>
#include "Demiurge.h"


Mixer::Mixer() {
   ESP_LOGE("Mixer", "Constructor: %x", (void *) this);
   for (auto &input : _data.inputs)
      input = nullptr;
   _signal.read_fn = mixer_read;
   _signal.data = &_data;
   _signal.extra1 = -1.0;
}

Mixer::~Mixer() = default;

void Mixer::configure(int number, Signal *source, Signal *control) {
   configASSERT(number > 0 && number <= DEMIURGE_MAX_MIXER_IN)
   auto *v = new Volume();
   v->configure(source, control);
   _data.inputs[number - 1] = &v->_signal;
}

float IRAM_ATTR mixer_read(signal_t *handle, uint64_t time) {
   auto *mixer = (mixer_t *) handle->data;
   if (time > handle->last_calc) {
      handle->last_calc = time;
      float output = 0;
      int counter = 0;
      if( handle->extra1 == -1.0 ){
         ESP_LOGE("Mixer", "Inputs: %x      %x      %x      %x", (void *) mixer->inputs[0], (void *) mixer->inputs[1], (void *) mixer->inputs[2], (void *) mixer->inputs[3] );
      }

      for (auto inp : mixer->inputs) {
         if (inp != nullptr) {
            output = output + inp->read_fn(inp, time);
            counter++;
         }
      }
      handle->extra1 = counter;
      if (mixer->inputs[0] != nullptr)
         handle->extra2 = mixer->inputs[0]->cached;
      if (mixer->inputs[1] != nullptr)
         handle->extra3 = mixer->inputs[1]->cached;
      if (mixer->inputs[2] != nullptr)
         handle->extra4 = mixer->inputs[2]->cached;
      output = output / counter;
      handle->cached = output;
      return output;
   }
   return handle->cached;
}
