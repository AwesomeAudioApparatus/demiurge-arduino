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
   for (auto &input : _data.inputs)
      input = nullptr;
   _signal.read_fn = mixer_read;
   _signal.data = &_data;
}

Mixer::~Mixer() = default;

void Mixer::configure(int number, Signal *source, Signal *control) {
   configASSERT(number > 0 && number <= DEMIURGE_MAX_MIXER_IN)
   auto *v = new Volume();
   v->configure(source, control);
   _data.inputs[number - 1] = &v->_signal;
}

int32_t IRAM_ATTR mixer_read(signal_t *handle, uint64_t time) {
   auto *mixer = (mixer_t *) handle->data;
   if( time > mixer->lastCalc ) {
      mixer->lastCalc = time;
      int32_t output = 0;
      for (auto inp : mixer->inputs) {
         if (inp != nullptr) {
            output = output + inp->read_fn(inp, time);
         }
      }
      mixer->cached = output;
      return output;
   }
   return mixer->cached;
}
