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

ControlPair attack(1);
ControlPair decay(2);
ControlPair sustain(3);
ControlPair release(4);

Adsr adsr;
GateInPort gate(1);
Inverter invert;

AudioOutPort out1(1);
AudioOutPort out2(2);

/*
 * Envelope generator.
 */
void setup() {
   disableCore0WDT();

   adsr.configureGate(&gate);
   adsr.configureAttack(&attack);
   adsr.configureDecay(&decay);
   adsr.configureSustain(&sustain);
   adsr.configureRelease(&release);

   invert.configure(&adsr);
   out1.configure(&invert);

   out2.configure(&adsr);

   Demiurge::runtime().begin(10000);
}

void loop() {

}
