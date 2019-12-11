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

#include "esp_log.h"
#include "Demiurge.h"
#include "Pan.h"


Pan::Pan() {
   _left = new PanChannel(this, 0.5);
   _right = new PanChannel(this, -0.5);
};

Pan::~Pan() {
   delete _left;
   delete _right;
}

void Pan::configure(Signal *input, Signal *control) {
   configASSERT(input != nullptr)
   _input = input;
   configASSERT(control != nullptr)
   _control = control;

   ((panchannel_t*) _left->_signal.data)->hostInput = &input->_signal;
   ((panchannel_t*) _right->_signal.data)->hostInput = &input->_signal;
   ((panchannel_t*) _left->_signal.data)->control = &control->_signal;
   ((panchannel_t*) _right->_signal.data)->control = &control->_signal;
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
   _signal.data = &_data;
   _data.factor = factor;
   _data.me = &_signal;
}

PanChannel::~PanChannel() = default;

float IRAM_ATTR panchannel_read(signal_t *handle, uint64_t time) {
   auto *panchannel = (panchannel_t *) handle->data;
   if( time > panchannel->lastCalc ) {
      panchannel->lastCalc = time;
      signal_t *panControl = panchannel->control;
      float control = panControl->read_fn(panControl, time);
      signal_t *hostInput = panchannel->hostInput;
      float input = hostInput->read_fn(hostInput, time);
      input = input * control;
      signal_t *data = panchannel->me;
      if (data->noRecalc) {
         float result = input * panchannel->factor;
         panchannel->cached = result;
         return result;
      }
      float result = (input * data->scale + data->offset) * panchannel->factor;
      panchannel->cached = result;
      return result;
   }
   return panchannel->cached;
}
