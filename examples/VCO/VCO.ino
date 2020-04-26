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

#define DEMIURGE_TYPE FLOAT

#include "Arduino.h"
#include "Demiurge.h"

ControlPair pair1(1);
ControlPair pair2(2);
AudioOutPort out1(1);
AudioOutPort out2(2);
Oscillator vco1(DEMIURGE_SINE);

/*
 * Simple VCO with sine wave on both outputs.
 */
void setup() {
   disableCore0WDT();

   vco1.configureFrequency(&pair1);
   vco1.configureAmplitude(&pair2);

   out1.configure(&vco1);
   out2.configure(&vco1);
   Demiurge::begin(48000);
}

void loop() {
}
