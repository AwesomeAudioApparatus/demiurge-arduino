
#include "Arduino.h"
#include "Demiurge.h"

AudioInPort in1(1);
AudioInPort in2(2);
AudioInPort in3(3);
AudioInPort in1(4);

Potentiometer pot1(1);
Potentiometer pot2(2);
Potentiometer pot3(3);
Potentiometer pot4(4);

AudioOutPort out1(1);
AudioOutPort out2(2);

Mixer mixer(4);

void setup()
{
   mixer.configureInput(1, in1, 1, 0.0 );
   mixer.configureScale(1, pot1);

   mixer.configureInput(2, in2, 1, 0.0 );
   mixer.configureScale(2, pot2);

   mixer.configureInput(3, in3, 1, 0.0 );
   mixer.configureScale(3, pot3);

   mixer.configureInput(4, in4, 1, 0.0 );
   mixer.configureScale(4, pot4);

   out1.configure(mixer)
   out2.configure(mixer)
}

void loop()
{

}