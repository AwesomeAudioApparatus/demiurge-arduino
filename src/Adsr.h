#ifndef _DEMIURGE_ADSR_H_
#define _DEMIURGE_ADSR_H_

#include "Signal.h"
#include "Threshold.h"

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

   double update(double time) override;

private:
   Signal *_gate;
   Signal *_trig;
   Signal *_attack;
   Signal *_decay;
   Signal *_sustain;
   Signal *_release;

   Threshold *_gateThreshold;
   Threshold *_trigThreshold;

   int stateMachine = 0;
   double doneAt = 0.0;
   double startedAt = 0.0;
   bool _currentTrig;
   bool _currentGate;

   double slopeTime(double voltage);
};


#endif
