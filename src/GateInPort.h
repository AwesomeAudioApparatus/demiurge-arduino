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

#ifndef _DEMIURGE_GATEPORT_H_
#define _DEMIURGE_GATEPORT_H_

#ifndef DEMIURGE_GATE_HIGH
#define DEMIURGE_GATE_HIGH 0x00400000  // 0x4000 in audio bits
#endif

#ifndef DEMIURGE_GATE_LOW
#define DEMIURGE_GATE_LOW 0x007FFF00   // 0x7FFF in audio bits
#endif

#include "Signal.h"

typedef struct {
   int position;
} gate_in_port_t;

float gateinport_read(signal_t *handle, uint64_t time);

class GateInPort : public Signal {

public:
   explicit GateInPort(int position);

   ~GateInPort() override;
private:
   gate_in_port_t _data{};
};


#endif