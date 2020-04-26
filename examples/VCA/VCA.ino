/*
  Copyright 2019-2020, Awesome Audio Apparatus.

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

#include "Arduino.h"
#include "Demiurge.h"

AudioInPort in1(1);
AudioInPort in2(2);
AudioInPort in3(3);
AudioInPort in4(4);

Potentiometer pot1(1);
Potentiometer pot2(2);
Potentiometer pot3(3);
Potentiometer pot4(4);

AudioOutPort out1(1);
AudioOutPort out2(2);

Mixer mixer(4);

void setup() {
   Demiurge::runtime().begin();

   mixer.configure(1, &in1, &pot1);

   mixer.configure(2, &in2, &pot2);

   mixer.configure(3, &in3, &pot3);

   mixer.configure(4, &in4, &pot4);

   out1.configure(&mixer);
   out2.configure(&mixer);
}

void loop() {

}
