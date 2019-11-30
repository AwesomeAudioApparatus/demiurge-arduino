
#include "Demiurge.h"

Potentiometer::Potentiometer(int position) {
   _position = position + DEMIURGE_CVINPUT_OFFSET;
}

double Potentiometer::read(double time) {
   return Demiurge::runtime()->inputs()->value[_position] * scale() + offset();
}
