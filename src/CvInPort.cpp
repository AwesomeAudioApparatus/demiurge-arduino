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


CvInPort::CvInPort(int position) {
   configASSERT(position > 0 && position <= 4)
   ESP_LOGE("CvInPort", "Constructor: %x", (void *) this);
   _data.position = position + DEMIURGE_CVINPUT_OFFSET;
   _signal.read_fn = cvinport_read;
   _signal.data = &_data;
   ESP_LOGE("CvInPort", "_signal: %x", this->_signal);
}

CvInPort::~CvInPort() {
   ESP_LOGE("CvInPort", "Destructor: %x", (void *) this);

}

float IRAM_ATTR cvinport_read(signal_t *handle, uint64_t time) {
   auto *cv = (cv_in_port_t *) handle->data;
   if (time > handle->last_calc) {
      handle->last_calc = time;
      float input = Demiurge::runtime().input(cv->position);
      float out = input;
      handle->cached = out;
      return out;
   }
   return handle->cached;
}
