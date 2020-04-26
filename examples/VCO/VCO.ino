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

#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE

#include "esp_log.h"

ControlPair pair1(1);
ControlPair pair2(2);
AudioOutPort out1(1);
AudioOutPort out2(2);
Oscillator vco1(DEMIURGE_SINE);

void setup() {
   disableCore0WDT();
   Serial.begin(115200);

   Serial.println("setting up vco");
   vco1.configureFrequency(&pair1);
   vco1.configureAmplitude(&pair2);

   Serial.println("setting up output ports");
   out1.configure(&vco1);
   out2.configure(&vco1);
   Demiurge::begin(48000);
   delay(100);
}

void loop() {
   auto &demiurge = Demiurge::runtime();

   double value1 = vco1._signal.extra1;
   double value2 = vco1._signal.extra2;
   double value3 = vco1._signal.extra3;
   double value4 = vco1._signal.extra4;
   Serial.print( "VCO: " );
   Serial.print(millis());
   Serial.print(", ");
   Serial.print(value1);
   Serial.print(", ");
   Serial.print(value2);
   Serial.print(", ");
   Serial.print(value3);
   Serial.print(", ");
   Serial.print(value4);

   for( int i=1; i<=8; i++ ) {
      Serial.print( ", " );
      Serial.print(demiurge.input(i));
   }

   Serial.println();
   delay(1021);
}
