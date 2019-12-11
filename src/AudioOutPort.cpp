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
   _data.me = &_signal;
   _data.position = position;
   _signal.read_fn = audiooutport_read;
   _signal.data = &_data;
}

AudioOutPort::~AudioOutPort() {
   if (_data.registered){
      Demiurge::runtime().unregisterSink(&_signal);
      _data.registered = false;
   }
};

void AudioOutPort::configure(Signal *input) {
   if (!_data.registered){
      Demiurge::runtime().registerSink(&_signal);
      _data.registered = true;
   }
   _input = input;
   _data.input = &input->_signal;
   _signal.data = &_data;
}

void AudioOutPort::configure(Signal *input, float scale, float offset) {
   configure(input);
   setScale(scale);
   setOffset(offset);
}

float IRAM_ATTR audiooutport_read(signal_t *handle, uint64_t time) {
   auto *port = (audio_out_port_t *) handle->data;
   if( time > port->lastCalc ) {
      port->lastCalc = time;
      signal_t *upstream = port->input;
      signal_fn fn = upstream->read_fn;
      float input = fn(upstream, time);
      signal_t *data = port->me;
      if (data->noRecalc)
         return input;
      float result = input * data->scale + data->offset;
      port->cached = result;
      return result;
   }
   return port->cached;
}

