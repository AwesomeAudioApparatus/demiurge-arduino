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
ControlPair pair3(3);
ControlPair pair4(4);

GateInPort gate(1);

AudioOutPort out1(1);
AudioOutPort out2(2);

Oscillator vco1(DEMIURGE_SAW);

Mixer mixer;
FixedSignal fixed(1.0);
Pan pan;

void setup() {
  disableCore0WDT();
  Serial.begin(115200);

  Serial.println("setting up mixer");
  mixer.configure(2, &pair1, &fixed);
  mixer.configure(1, &pair2, &fixed);

  Serial.println("setting up vco");
  vco1.configureFrequency(&mixer);
  vco1.configureAmplitude(&pair3);

  Serial.println("setting up pan");
  pan.configure(&vco1, &mixer);

  Serial.println("setting up output ports");
  out1.configure(pan.outputLeft());
  out2.configure(pan.outputRight());
  Demiurge::begin();
  delay(100);
}

void loop() {
  auto &demiurge = Demiurge::runtime();   

  double value1a = vco1._signal.cached;
//  double value2 = pan.outputLeft()->_signal.cached;
//  double value3 = out1._signal.cached;
  Serial.print( "VCO: " );
  Serial.print(millis());
  Serial.print(", ");
  Serial.print(value1a);

  for( int i=0; i<8; i++ ) {
    Serial.print( ", " );
    Serial.print(demiurge.input(i));
  }

  Serial.println();
  delay(1000);
}
