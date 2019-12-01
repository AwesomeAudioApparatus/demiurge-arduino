#ifndef _DEMIURGE_MIXER_H_
#define _DEMIURGE_MIXER_H_

#include <list>
#include "Signal.h"
#include "Passthru.h"
#include "Volume.h"

class Mixer : public Signal {

public:
   explicit Mixer(int numberOfInputs);

   ~Mixer() ;

   void configure(int number, Signal *source, Signal *control);

   double update(double time) override;

private:
   Volume **_inputs;
   int _numberOfInputs;
};


#endif
