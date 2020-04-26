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

#include <freertos/FreeRTOS.h>
#include <esp_system.h>
#include <esp_log.h>
#include "ControlPair.h"

ControlPair::ControlPair(int position) {
   ESP_LOGD("ControlPair", "Constructor: %llx at position %d", (uint64_t) this, position );
   configASSERT(position > 0 && position <= 4)
   _potentiometer = new Potentiometer(position);
   _cvIn = new CvInPort(position);
   _potentiometerScale = 1.0;
   _cvScale = 1.0;
   _signal.read_fn = controlpair_read;
   _signal.data = &_data;
   _data.potentiometer = &_potentiometer->_signal;
   _data.cv = &_cvIn->_signal;
}

ControlPair::~ControlPair() {
   delete _potentiometer;
   delete _cvIn;
}

void ControlPair::setPotentiometerScale(float scale) {
   _potentiometerScale = scale;
}

void ControlPair::setCvScale(float scale) {
   _cvScale = scale;
}

float IRAM_ATTR controlpair_read(signal_t *handle, uint64_t time) {
   auto *control = (control_pair_t *) handle->data;
   if( time > handle->last_calc ) {
      handle->last_calc = time;

      signal_t *pot = control->potentiometer;
      float potIn = pot->read_fn(pot, time);

      signal_t *cv = control->cv;
      float cvIn = cv->read_fn(cv, time);
      float result = (potIn + cvIn) / 2;
      handle->cached = result;
      return result;
   }
   return handle->cached;
}
