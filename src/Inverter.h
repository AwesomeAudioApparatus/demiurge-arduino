
#ifndef _DEMIURGE_INVERTER_H_
#define _DEMIURGE_INVERTER_H_

#include "Signal.h"

class Inverter : public Signal {

public:
   Inverter();
   ~Inverter() override;

   explicit Inverter(double midpoint);

   Inverter(double midpoint, double scale);

   void configure(Signal *input);

   double update(double time) override;

private:
   double _midpoint;
   double _scale;
   Signal* _input;
};


#endif
