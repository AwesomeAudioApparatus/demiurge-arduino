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

#include "Demiurge.h"

CvInPort::CvInPort(int position) {
   configASSERT(position > 0 && position <= 4)
   _data.position = position + DEMIURGE_CVINPUT_OFFSET;
   _signal.read_fn = cvinport_read;
   _signal.data = &_data;
}

CvInPort::~CvInPort() = default;

float IRAM_ATTR cvinport_read(signal_t *handle, uint64_t time) {
   auto *cv = (cv_in_port_t *) handle->data;
   if (time > cv->lastCalc) {
      cv->lastCalc = time;
      float result = Demiurge::runtime().inputs()[cv->position];
      cv->cached = result;
      return result;
   }
   return cv->cached;
}
