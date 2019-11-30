#ifndef DEMIURGE_AUDIOINPORT_H
#define DEMIURGE_AUDIOINPORT_H

#include "Signal.h"

class AudioInPort : public Signal {

public:
   AudioInPort(int position);
   virtual ~AudioInPort() = 0;

   void configure( double scale, double offset );

private:
   double read(double time) override;

   int _position;
};


#endif
