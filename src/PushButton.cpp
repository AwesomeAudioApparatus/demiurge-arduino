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

#include <freertos/FreeRTOS.h>
#include <esp_system.h>
#include <esp_log.h>
#include "PushButton.h"
#include "Demiurge.h"

PushButton::PushButton(int position) {
   ESP_LOGD("Potentiometer", "Constructor: %llx at position %d", (uint64_t) this, position );
   configASSERT(position > 0 && position <= 4 )
   _data.position = position + DEMIURGE_PUSHBUTTON_OFFSET;
   _signal.data = &_data;
   _signal.read_fn = pushbutton_read;

}

PushButton::~PushButton() = default;

float IRAM_ATTR pushbutton_read(signal_t *handle, uint64_t time) {
   if( time > handle->last_calc ){
      auto *button = (pushbutton_t *) handle->data;
      float result = Demiurge::runtime().gpio(button->position);
      handle->cached = result;
      return result;
   }
   return handle->cached;
}
