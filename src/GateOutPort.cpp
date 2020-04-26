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
#include "GateOutPort.h"
#include "Clipping.h"
#include "Demiurge.h"

GateOutPort::GateOutPort(int position) {
   ESP_LOGD("GateOutPort", "Constructor: %llx at position %d", (uint64_t) this, position );
   configASSERT(position > 0 && position <= 2)
   _data.me = &_signal;
   _data.position = position;
   _signal.read_fn = gateoutport_read;
   _signal.data = &_data;
}

GateOutPort::~GateOutPort() {
   if (_data.registered){
      Demiurge::runtime().unregisterSink(&_signal);
      _data.registered = false;
   }
};

void GateOutPort::configure(Signal *input) {
   if (!_data.registered){
      Demiurge::runtime().registerSink(&_signal);
      _data.registered = true;
   }
   _input = input;
   _data.input = &input->_signal;
   _signal.data = &_data;
}

float IRAM_ATTR gateoutport_read(signal_t *handle, uint64_t time) {
   if( time > handle->last_calc ) {
      auto *port = (cv_out_port_t *) handle->data;
      handle->last_calc = time;
      signal_t *upstream = port->input;
      signal_fn fn = upstream->read_fn;
      float result = clipGate(fn(upstream, time));
      handle->cached = result;
      return result;
   }
   return handle->cached;
}

