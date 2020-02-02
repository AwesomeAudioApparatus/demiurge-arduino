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
#include "AudioInPort.h"


AudioInPort::AudioInPort(int position) {
   configASSERT(position > 0 && position <= 4)
   _data.position = position;
   _signal.read_fn = audioinport_read;
   _signal.data = &_data;
}

AudioInPort::~AudioInPort() = default;

float IRAM_ATTR audioinport_read(signal_t *handle, uint64_t time) {
   auto *port = (audio_in_port_t *) handle->data;
   if (time > handle->last_calc) {
      handle->last_calc = time;
      float result = Demiurge::runtime().input(port->position);
      handle->cached = result;
      return result;
   }
   return handle->cached;
}