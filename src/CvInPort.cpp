
#include "Demiurge.h"

CvInPort::CvInPort(int position) {
   _position = position + DEMIURGE_CVINPUT_OFFSET;
}

double CvInPort::read(double time) {
   return Demiurge::runtime()->inputs()->value[_position] * scale() + offset();
}
