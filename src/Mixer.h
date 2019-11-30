#ifndef _DEMIURGE_MIXER_H_
#define _DEMIURGE_MIXER_H_

#include <list>
#include "Signal.h"
#include "ScaledSignal.h"

class Mixer : public Signal {

public:
   explicit Mixer(int numberOfInputs);

   ~Mixer();

   void configureInput(int number, Signal *source, double scale, double offet);

   void configureVolume(int number, Signal *source, double scale, double offset);

private:
   ScaledSignal **_inputs;
   Signal **_volumes;
   int _numberOfInputs;
};


#endif
