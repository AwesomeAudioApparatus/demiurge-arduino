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

#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE

#include "esp_log.h"

ControlPair pair1(1);
ControlPair pair2(2);
ControlPair pair3(3);
ControlPair pair4(4);

GateInPort gate(1);

AudioOutPort out1(1);
AudioOutPort out2(2);

Oscillator vco1(DEMIURGE_SINE);

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
   out1.configure(pan.outputLeft());
   out2.configure(pan.outputRight());
   Demiurge::begin();
}

void loop() {

   Demiurge::runtime().printReport();
   uint16_t *inputs = Demiurge::runtime().rawAdc();
   for( int i=0; i < 8; i++ ) {
     Serial.print( inputs[i] );
     Serial.print( "    " );
   }
   Serial.println();
   
   Serial.print( Demiurge::runtime().output1() );
   Serial.print( "    " );
   Serial.print( Demiurge::runtime().dac1() );
   Serial.print( "    " );
   Serial.print( Demiurge::runtime().output2() );
   Serial.print( "    " );
   Serial.print( Demiurge::runtime().dac2() );
   Serial.println();

   double value1 = ((oscillator_t *) vco1._signal.data)->cached;
   double value2 = ((panchannel_t *) pan.outputLeft()->_signal.data)->cached;
   double value3 = ((audio_out_port_t *) out1._signal.data)->cached;
   Serial.print( "VCO: " );
   Serial.print(value1);
   Serial.print( "    Pan:" );
   Serial.print(value2);
   Serial.print( "    Out:" );
   Serial.print(value3);
   Serial.println();
   delay(872);
}
