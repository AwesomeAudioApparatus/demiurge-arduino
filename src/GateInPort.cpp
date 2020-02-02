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
#include "driver/gpio.h"

static const int DEMIURGE_GATE_GPIO[1] = {27};

GateInPort::GateInPort(int position) {
   configASSERT(position > 0 && position <= 1 )
   _data.position = position-1;
   _signal.data = &_data;
}

GateInPort::~GateInPort() = default;

float IRAM_ATTR gateinport_read(signal_t *handle, uint64_t time){
   auto *gate = (gate_in_port_t *) handle->data;
   if( time > handle->last_calc ) {
      handle->last_calc = time;
      bool state = Demiurge::runtime().gpio(DEMIURGE_GATE_GPIO[gate->position]);
      handle->cached = state ? DEMIURGE_GATE_HIGH : DEMIURGE_GATE_LOW;
   }
   return handle->cached;
}
