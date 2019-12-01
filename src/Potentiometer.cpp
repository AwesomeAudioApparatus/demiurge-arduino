
#include "Demiurge.h"


Potentiometer::Potentiometer(int position) {
   configASSERT(position > 0 && position <= 4 )
   _position = position + DEMIEURGE_POTENTIOMETER_OFFSET;
}

Potentiometer::~Potentiometer() = default;

double Potentiometer::update(double time) {
   return Demiurge::runtime().inputs()[_position];
}
