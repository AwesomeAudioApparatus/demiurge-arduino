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
#include "Pan.h"


Pan::Pan() {
   _left = new PanChannel(this, 0.05);
   _right = new PanChannel(this, -0.05);
};

void Pan::configure(Signal *input, Signal *control) {
   _input = input;
   _control = control;
}

Signal *Pan::outputLeft() {
   return _left;
}

Signal *Pan::outputRight() {
   return _right;
}

PanChannel::PanChannel(Pan *host, float factor) {
   _host = host;
   _signal.read_fn = panchannel_read;
   _data.factor = factor;
   _data.host = &host->_input->_signal;
   _data.control = &host->_control->_signal;
}

PanChannel::~PanChannel() = default;

float IRAM_ATTR panchannel_read(void *handle, uint64_t time) {
   auto *panchannel = (panchannel_t *) handle;
   float control = panchannel->control->read_fn(panchannel->control, time);
   float input = panchannel->host->read_fn(panchannel->host, time);
   input = input * control;
   signal_t *data = panchannel->me;
   if (data->noRecalc)
      return input * panchannel->factor ;
   return (input * data->scale + data->offset) * panchannel->factor ;
}
