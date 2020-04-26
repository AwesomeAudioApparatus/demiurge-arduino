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
#include "FixedSignal.h"

FixedSignal::FixedSignal(float value) {
   ESP_LOGD("FixedSignal", "Constructor: %llx with value %f", (uint64_t) this, value );
   _data.value = value;
   _signal.read_fn = fixedsignal_read;
   _signal.data = &_data;
}

FixedSignal::~FixedSignal() = default;

float IRAM_ATTR fixedsignal_read(signal_t *handle, uint64_t time)
{
   auto *fixed = (fixed_signal_t *) handle->data;
   return fixed->value;
}
