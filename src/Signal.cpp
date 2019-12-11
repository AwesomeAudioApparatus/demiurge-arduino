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

#include <driver/gpio.h>
#include "Signal.h"

Signal::Signal() noexcept {
   _signal.read_fn = nullptr;
   _signal.noRecalc = true;
   _signal.scale = 1.0f;
   _signal.offset = 0.0f;
}

Signal::~Signal() {}

float Signal::scale() {
   return _scale;
}

float Signal::offset() {
   return _offset;
}

void Signal::setScale(float scale) {
   _scale = scale;
   if (_scale == 1.0 && _offset == 0.0)
      _noRecalc = true;
   _signal.scale = scale;
   _signal.noRecalc = _noRecalc;
}

void Signal::setOffset(float offset) {
   _offset = offset;
   if (_scale == 1.0 && _offset == 0.0)
      _noRecalc = true;
   _signal.offset = offset;
   _signal.noRecalc = _noRecalc;
}
