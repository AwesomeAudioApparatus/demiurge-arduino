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

AudioOutPort::AudioOutPort(int position) {
   configASSERT(position > 0 && position <= 2)
   _position = position;
}

AudioOutPort::~AudioOutPort() {
   if (_registered)
      Demiurge::runtime().unregisterSink(this);
};

void AudioOutPort::configure(Signal *input) {
   if (!_registered)
      Demiurge::runtime().registerSink(this);
   _input = input;
}

void AudioOutPort::configure(Signal *input, float scale, float offset) {
   _input = input;
   setScale(scale);
   setOffset(offset);
}

float IRAM_ATTR AudioOutPort::update(uint64_t time) {
   return _input->read(time);
}

void IRAM_ATTR AudioOutPort::tick(uint64_t time) {
   float voltage = update(time);
   Demiurge::runtime().setDAC(_position, voltage);
}