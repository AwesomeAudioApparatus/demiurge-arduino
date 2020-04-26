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
#include "CvInPort.h"
#include "Clipping.h"


CvInPort::CvInPort(int position) {
   ESP_LOGD("CvInPort", "Constructor: %llx at position %d", (uint64_t) this, position);
   configASSERT(position > 0 && position <= 4)
   _data.position = position + DEMIURGE_CVINPUT_OFFSET;
   _signal.read_fn = cvinport_read;
   _signal.data = &_data;
   ESP_LOGD("CvInPort", "_signal: %x", this->_signal);
}

CvInPort::~CvInPort() {
   ESP_LOGD("CvInPort", "Destructor: %llx", (uint64_t) this);

}

float IRAM_ATTR cvinport_read(signal_t *handle, uint64_t time) {
   auto *cv = (cv_in_port_t *) handle->data;
   if (time > handle->last_calc) {
      handle->last_calc = time;
      float input = clipCV(Demiurge::runtime().input(cv->position));
      handle->cached = input;
      return input;
   }
   return handle->cached;
}
