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

#include <stdint.h>
#include "OctavePerVolt.h"

int32_t octave_frequencyOf(int32_t voltage) {
   // TODO:
//   return 440 / pow(2, 2.75) * pow(2, voltage);
   return 440;
}

int32_t octave_voltageOf(int32_t frequency) {
   // TODO:
   return 1;
}
