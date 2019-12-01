
#include "Demiurge.h"

Potentiometer::Potentiometer(int position) {
   _position = position + DEMIURGE_CVINPUT_OFFSET;
}

double Potentiometer::update(double time) {
   return Demiurge::runtime().inputs()[_position];
}
