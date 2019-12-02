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

#include "Signal.h"

Signal::~Signal() {}

double Signal::scale() {
   return _scale;
}

double Signal::offset() {
   return _offset;
}

void Signal::setScale(double scale) {
   _scale = scale;
   if (_scale == 1.0 && _offset == 0.0)
      _noRecalc = true;
}

void Signal::setOffset(double offset) {
   _offset = offset;
   if (_scale == 1.0 && _offset == 0.0)
      _noRecalc = true;
}

double Signal::read(double time) {
   if (time > _lastRead) {
      if (_noRecalc)
         _output = update(time);
      else
         _output = update(time) * _scale + _offset;
      _lastRead = time;
   }
   return _output;
}

double Signal::currentValue() {
   return _output;
}
