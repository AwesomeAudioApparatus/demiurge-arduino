
#include "Demiurge.h"

static const int DEMIURGE_GATE_GPIO[1] = {27};

GatePort::GatePort(int position) {
   configASSERT(position > 0 && position <= 1 )
   _position = position-1;
}

GatePort::~GatePort() = default;

double GatePort::update(double time) {
   bool state = Demiurge::runtime().gpio(DEMIURGE_GATE_GPIO[_position]);
   if( state )
   {
      return DEMIURGE_GATE_HIGH;
   }
   return 0.0;
}
