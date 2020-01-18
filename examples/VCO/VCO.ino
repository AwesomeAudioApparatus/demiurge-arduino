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

#include "Arduino.h"
#include "Demiurge.h"
#include <soc/mcpwm_reg.h>

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

#define ADC_CS 5
#define ADC_CLK 18
#define ADC_MISO 19
#define ADC_MOSI 23

void setup() {
  disableCore0WDT();
  Serial.begin(115200);
  pinMode(ADC_CS, OUTPUT); digitalWrite(ADC_CS, HIGH);
  pinMode(ADC_MOSI, OUTPUT); digitalWrite(ADC_MOSI, LOW);
  pinMode(ADC_MISO, INPUT);
  pinMode(ADC_CLK, OUTPUT); digitalWrite(ADC_CLK, LOW);

  Serial.println("setting up mixer");
  mixer.configure(2, &pair1, &fixed);
  mixer.configure(1, &pair2, &fixed);

  Serial.println("setting up vco");
  vco1.configureFrequency(&mixer);
  vco1.configureAmplitude(&pair3);

  Serial.println("setting up pan");
  pan.configure(&vco1, &mixer);

  Serial.println("setting up output ports");
//  out1.configure(pan.outputLeft());
//  out2.configure(pan.outputRight());
  out1.configure(&vco1);
  out2.configure(&vco1);
  Demiurge::begin();
  delay(100);
}

void loop() {
  auto &demiurge = Demiurge::runtime();   

  double value1a = ((oscillator_t *) vco1._signal.data)->cached;
  double value1b = ((oscillator_t *) vco1._signal.data)->x;
  double value1c = ((oscillator_t *) vco1._signal.data)->y;
//  double value2 = ((panchannel_t *) pan.outputLeft()->_signal.data)->cached;
//  double value3 = ((audio_out_port_t *) out1._signal.data)->cached;
  Serial.print( "VCO: " );
  Serial.print(value1a);
  Serial.print( ", " );
  Serial.print(value1b);
  Serial.print( ", " );
  Serial.print(value1c);
  Serial.print( ", " );
  Serial.print(demiurge._dac->descs->buf[0]);
  Serial.print( ", " );
  Serial.print(demiurge._dac->descs->buf[1]);
  Serial.print( ", " );
  Serial.print(demiurge._dac->descs->buf[2]);
  Serial.print( ", " );
  Serial.print(demiurge._dac->descs->buf[3]);

  Serial.print( ", " );
  Serial.print(demiurge.outputs()[0]);
  Serial.print( ", " );
  Serial.print(demiurge.outputs()[1]);
//  Serial.print( "    Pan:" );
//  Serial.print(value2);
//  Serial.print( "    Out:" );
//  Serial.print(value3);
  Serial.println();

  delay(50);
}
