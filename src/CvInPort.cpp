
#include "Demiurge.h"
#include "CvInPort.h"


CvInPort::CvInPort(int position) {
   _position = position + DEMIURGE_CVINPUT_OFFSET;
}

CvInPort::~CvInPort() = default;

double CvInPort::read(double time) {
   return Demiurge::runtime()->inputs()->value[_position] * scale() + offset();
}
