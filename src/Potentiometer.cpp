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


#include <esp_log.h>
#include "Demiurge.h"


Potentiometer::Potentiometer(int position) {
   ESP_LOGD("Potentiometer", "Constructor: %x at position %d", (void *) this, position );
   configASSERT(position > 0 && position <= 4 )
   _data.position = position + DEMIEURGE_POTENTIOMETER_OFFSET;
   _signal.data = &_data;
   _signal.read_fn = potentiometer_read;
}

Potentiometer::~Potentiometer() = default;

float IRAM_ATTR potentiometer_read(signal_t *handle, uint64_t time) {
   auto *port = (potentiometer_t *) handle->data;
   float result = Demiurge::runtime().input(port->position);
   return result;
}
