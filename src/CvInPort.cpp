
#include "Demiurge.h"

CvInPort::CvInPort(int position) {
   configASSERT(position > 0 && position <= 4 )
   _position = position + DEMIURGE_CVINPUT_OFFSET;
}

CvInPort::~CvInPort() = default;

double CvInPort::update(double time) {
   return Demiurge::runtime().inputs()[_position];
}
