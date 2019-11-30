
#include "Arduino.h"
#include "Demiurge.h"

CvInPort cv1(1);
CvInPort cv2(2);
CvInPort cv3(3);
CvInPort cv4(4);

GatePort gate(1);

AudioOutPort out1(1);
AudioOutPort out2(2);

VCO vco1();

Mixer mixer(2);

Pan pan();

void setup() {
   Demiurge::initialize(20000);

   mixer.configureInput(2, &cv1, 0.1, 0.0);
   mixer.configureInput(1, &cv2, 1.0, 0.0);

   vco1.configureFrequency(&mixer);
   vco1.configureAmplitude(&cv3);
   vco1.configureMute(&gate);

   pan.configure(&vco1);
   out1.configure(pan.outputLeft());
   out2.configure(pan.outputRight());
}

void loop() {

}