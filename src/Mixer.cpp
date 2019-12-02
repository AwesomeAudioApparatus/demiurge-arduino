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

#include <malloc.h>
#include <esp_system.h>
#include <esp_task.h>
#include "Mixer.h"


Mixer::Mixer(int numberOfInputs) {
   _inputs = static_cast<Volume **>(malloc(sizeof(Volume *) * numberOfInputs));
   _numberOfInputs = numberOfInputs;
}

Mixer::~Mixer() {
   for (int i = 0; i < _numberOfInputs; i++) {
      delete _inputs[i];
   }
   free(_inputs);
}

void Mixer::configure(int number, Signal *source, Signal *control) {
   configASSERT(number > 0 && number <= _numberOfInputs)
   Volume *v = new Volume();
   v->configure(source, control);
   _inputs[number-1] = v;
}

double IRAM_ATTR Mixer::update(uint64_t time) {
   double output = 0.0;
   for (int i = 0; i < _numberOfInputs; i++) {
      Volume *inp = _inputs[i];
      if (inp != nullptr) {
         output = output + inp->read(time);
      }
   }
   return output;
}
