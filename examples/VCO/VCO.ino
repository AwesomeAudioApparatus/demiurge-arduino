
#include "Arduino.h"
#include "Demiurge.h"

#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include "esp_log.h"

CvInPort cv1(1);
CvInPort cv2(2);
CvInPort cv3(3);
CvInPort cv4(4);

GatePort gate(1);

AudioOutPort out1(1);
AudioOutPort out2(2);

VCO vco1(DEMIURGE_SINE);

Mixer mixer(2);
FixedSignal fixed(1.0);
Pan pan;

void setup() {
  Serial.begin(115200);
  Serial.println("setup()");
  Demiurge::runtime().begin();
  Serial.println("runtime() called");

  Serial.println("setting up mixer");
   mixer.configure(2, &cv1, &fixed);
   mixer.configure(1, &cv2, &fixed);

  Serial.println("setting up vco");
   vco1.configureFrequency(&mixer);
   vco1.configureAmplitude(&cv3);

  Serial.println("setting up pan");
   pan.configure(&vco1,&cv4);
   out1.configure(pan.outputLeft());
   out2.configure(pan.outputRight());
}

void loop() {
  Serial.println("loop");
  delay(100);
}
