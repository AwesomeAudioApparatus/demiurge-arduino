
#include "Arduino.h"
#include "Demiurge.h"

ControlPair attack(1);
ControlPair decay(2);
ControlPair sustain(3);
ControlPair release(4);

Adsr adsr;

GatePort gate;

Inverter invert;

AudioOutPort out1(1);
AudioOutPort out2(2);

void setup() {
   Demiurge::initialize();

   adsr.configureGate(&gate);
   adsr.configureAttack(&attack);
   adsr.configureDecay(&decay);
   adsr.configureSustain(&sustain);
   adsr.configureRelease(&release);

   out1.configure(&adsr);

   invert.configure(&adsr);

   out2.configure(&invert);
}

void loop() {

}
