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
   _signal.read_fn = audiooutport_read;
}

AudioOutPort::~AudioOutPort() {
   if (_registered)
      Demiurge::runtime().unregisterSink((audio_out_port_t *)(&_signal));
};

void AudioOutPort::configure(Signal *input) {
   if (!_registered)
      Demiurge::runtime().registerSink((audio_out_port_t *)(&_signal));
   _input = input;
   _data.input = &input->_signal;
}

void AudioOutPort::configure(Signal *input, float scale, float offset) {
   configure(input);
   setScale(scale);
   setOffset(offset);
}

float IRAM_ATTR audiooutport_read(void *handle, uint64_t time) {
   auto *port = (audio_out_port_t *) handle;
   signal_t *upstream = port->input;
   signal_fn fn = upstream->read_fn;
   float input = fn(upstream, time);
   signal_t *data = port->me;
   if (data->noRecalc)
      return input;
   return input * data->scale + data->offset;
}

