
#include "Arduino.h"
#include "Demiurge.h"

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
   Demiurge::initialize();

   mixer.configure(2, &cv1, &fixed);
   mixer.configure(1, &cv2, &fixed);

   vco1.configureFrequency(&mixer);
   vco1.configureAmplitude(&cv3);

   pan.configure(&vco1,&cv4);
   out1.configure(pan.outputLeft());
   out2.configure(pan.outputRight());
}

void loop() {

}
