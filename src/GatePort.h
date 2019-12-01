
#ifndef _DEMIURGE_GATEPORT_H_
#define _DEMIURGE_GATEPORT_H_

#ifndef DEMIURGE_GATE_HIGH
#define DEMIURGE_GATE_HIGH 5.0
#endif

#ifndef DEMIURGE_GATE_LOW
#define DEMIURGE_GATE_LOW 0.0
#endif

#include "Signal.h"

class GatePort : public Signal {

public:
   explicit GatePort(int position);

   ~GatePort() ;

   double update(double time) override;

private:

   int _position;
};


#endif
