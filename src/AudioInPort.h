#ifndef DEMIURGE_AUDIOINPORT_H
#define DEMIURGE_AUDIOINPORT_H

#include "Signal.h"

class AudioInPort : Signal {

public:
   AudioInPort(int position);
   virtual ~AudioInPort();

   void configure( double scale, double offset );
};


#endif
