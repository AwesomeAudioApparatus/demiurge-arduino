
#ifndef DEMIURGE_PAN_H
#define DEMIURGE_PAN_H

#include "Signal.h"
#include "GhostSignal.h"

class Pan : SoundProcessor {

public:
   Pan();

   void configure(Signal *input);

   Signal *outputLeft();

   Signal *outputRight();

private :
   Signal* _input;
   GhostSignal _left;
   GhostSignal _right;
};


#endif
