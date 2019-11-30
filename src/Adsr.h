#ifndef _DEMIURGE_ADSR_H_
#define _DEMIURGE_ADSR_H_

#include "Signal.h"

class Adsr : public Signal {

public:
   Adsr();

   ~Adsr();

   void configureGate(Signal *gate);

   void configureAttack(Signal *control);

   void configureDecay(Signal *control);

   void configureSustain(Signal *control);

   void configureRelease(Signal *control);

   void configureTrig(Signal *trig);

   double attack();

   double decay();

   double sustain();

   double release();

private:
   double _attack;
   double _decay;
   double _sustain;
   double _release;
};


#endif
