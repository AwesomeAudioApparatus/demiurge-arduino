
#ifndef DEMIURGE_GHOSTSIGNAL_H
#define DEMIURGE_GHOSTSIGNAL_H


#include "Signal.h"

class GhostSignal : public Signal {

public:
   GhostSignal();
   ~GhostSignal();

   void setValue(double value);

   double read(double time) override;

private:
   double _value = 0.0;
};


#endif
