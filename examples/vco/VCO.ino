

#include "Demiurge.h"

CvInPort cv1 = CvInPort(1);
CvInPort cv2 = CvInPort(2);
CvInPort cv3 = CvInPort(3);
CvInPort cv4 = CvInPort(4);

GatePort gate = GatePort(1);

AudioOutPort out1 = AudioOutPort(1);
AudioOutPort out2 = AudioOutPort(2);

VCO vco1();
Mixer mixer(2);
Pan pan;

void setup()
{
   mixer.configureInput(2, cv1, 0.1, 0.0 );
   mixer.configureInput(1, cv2, 1.0, 0.0 );

   vco1.configureFrequency( mixer );
   vco1.configureAmplitude( cv3 );
   vco1.configureMute( gate );

   out1.configure(pan.outputLeft())
   out2.configure(pan.outputRight())
}

void loop()
{

}