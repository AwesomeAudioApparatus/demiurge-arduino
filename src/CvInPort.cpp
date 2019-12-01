
#include "Demiurge.h"
#include "CvInPort.h"


CvInPort::CvInPort(int position) {
   _position = position + DEMIURGE_CVINPUT_OFFSET;
}

CvInPort::~CvInPort() = default;

double CvInPort::update(double time) {
   return Demiurge::runtime().inputs()[_position];
}
