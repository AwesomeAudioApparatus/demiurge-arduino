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

#include <esp_task.h>
#include "Demiurge.h"

Volume::Volume() {
}

Volume::~Volume() {

}

void Volume::configure(Signal *input, Signal *control) {
   configASSERT(input != nullptr && control != nullptr)
   _input = input;
   _control = control;
}

double IRAM_ATTR Volume::update(uint64_t time) {
   double in = _input->read(time);
   double control = _control->read(time);
   double factor = 0.05 * control + 0.5;
   return in * factor;
}
