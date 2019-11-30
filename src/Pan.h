
#ifndef DEMIURGE_PAN_H
#define DEMIURGE_PAN_H

#include "Signal.h"

class Pan : SoundProcessor {

public:
   Pan();
   ~Pan();

   Signal* outputLeft();
   Signal* outputRight();
};


#endif
