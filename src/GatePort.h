
#ifndef _DEMIURGE_GATEPORT_H_
#define _DEMIURGE_GATEPORT_H_

#include "Signal.h"

class GatePort : public Signal {

public:
   GatePort(int position);

   ~GatePort();
};


#endif
