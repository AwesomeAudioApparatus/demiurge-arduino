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
}

AudioInPort::~AudioInPort() = default;

void AudioInPort::configure(float scale, float offset) {
   setScale(scale);
   setOffset(offset);
}

float IRAM_ATTR audioinport_read(void *handle, uint64_t time) {
   auto *port = (audio_in_port_t *) handle;
   return Demiurge::runtime().inputs()[port->position];
}