#ifndef DEMIURGE_SCALEDSIGNAL_H
#define DEMIURGE_SCALEDSIGNAL_H

#include "Signal.h"

class ScaledSignal : public Signal {

public:
   ScaledSignal(Signal* wrapped, double scale, double offset );

   double read(double time) override;

private:
   Signal *_wrapped;
};


#endif
