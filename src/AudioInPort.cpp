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
   configASSERT(position > 0 && position <= 2)
   _data.position = position;
   _signal.read_fn = audioinport_read;
   _signal.data = &_data;
}

AudioInPort::~AudioInPort() = default;

void AudioInPort::configure(int32_t scale, int32_t offset) {
   setScale(scale);
   setOffset(offset);
}

int32_t IRAM_ATTR audioinport_read(signal_t *handle, uint64_t time) {
   auto *port = (audio_in_port_t *) handle->data;
   if( time > port->lastCalc ) {
      port->lastCalc = time;
      int32_t result = Demiurge::runtime().inputs()[port->position];
      port->cached = result;
      return result;
   }
   return port->cached;
}