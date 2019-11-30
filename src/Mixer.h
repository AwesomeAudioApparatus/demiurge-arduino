#ifndef _DEMIURGE_MIXER_H_
#define _DEMIURGE_MIXER_H_

#include "Signal.h"

class Mixer : Signal {

public:
   Mixer(int numberOfInputs);
   ~Mixer();

   void configureInput( int number, Signal* source, double scale, double offet);

   void configureScale( int number, Signal* source);

};


#endif
