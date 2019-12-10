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

#include <esp_task.h>
#include "Demiurge.h"

Volume::Volume() {
   _signal.read_fn = volume_read;
}

Volume::~Volume() = default;

void Volume::configure(Signal *input, Signal *control) {
   configASSERT(input != nullptr && control != nullptr)
   _input = input;
   _control = control;
   _data.input = &input->_signal;
   _data.control = &control->_signal;
}

float IRAM_ATTR volume_read(void *handle, uint64_t time) {
   auto *me = (volume_t *) handle;
   float control = me->control->read_fn(me->control, time);
   float in = me->control->read_fn(me->control, time);
   float factor = 0.05 * control + 0.5;
   return in * factor;
}
