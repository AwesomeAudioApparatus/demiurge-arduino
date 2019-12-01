
#ifndef DEMIURGE_VOLUME_H
#define DEMIURGE_VOLUME_H


#include "Signal.h"

class Volume : public Signal {

public:
   Volume();

   ~Volume() ;

   void configure( Signal *input, Signal *control);

   double update(double time) override;

private:
   Signal *_input;
   Signal *_control;
};


#endif
