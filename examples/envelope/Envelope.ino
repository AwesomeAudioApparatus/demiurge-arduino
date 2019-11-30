
#include "Arduino.h"
#include "Demiurge.h"

ControlPair attack(1);
ControlPair decay(2);
ControlPair sustain(3);
ControlPair release(4);

Adsr adsr();

GatePort gate();

Inverter invert();

AudioOutPort out1(1);
AudioOutPort out2(2);

void setup() {
   Demiurge::initialize(20000);

   adsr.configureGate(&gate);
   adsr.configureAttack(&attack);
   adsr.configureDecay(&decay);
   adsr.configureSustain(&sustain);
   adsr.configureRelease(&release);

   mixer.configureInput(1, &in1, 1, 0.0);
   mixer.configureScale(1, &pot1);

   mixer.configureInput(2, &in2, 1, 0.0);
   mixer.configureScale(2, &pot2);

   mixer.configureInput(3, &in3, 1, 0.0);
   mixer.configureScale(3, &pot3);

   mixer.configureInput(4, &in4, 1, 0.0);
   mixer.configureScale(4, &pot4);

   out1.configure(&mixer);

   invert.configure(&mixer);

   out2.configure(&invert);
}

void loop() {

}