#ifndef _DEMIURGE_AUDIOINPORT_H_
#define _DEMIURGE_AUDIOINPORT_H_

#include "Signal.h"

class AudioInPort : public Signal {

public:
   explicit AudioInPort(int position);
   ~AudioInPort();

   void configure( double scale, double offset );

   double update(double time) override;

private:
   int _position;
};


#endif
