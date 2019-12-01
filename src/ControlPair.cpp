
#include "Demiurge.h"

ControlPair::ControlPair(int position) {
   configASSERT(position > 0 && position <= 8 )
   _potentiometer = new Potentiometer(position);
   _cvIn = new CvInPort(position);
   _potentiometerScale = 1.0;
   _cvScale = 1.0;
}

ControlPair::~ControlPair() {
   delete _potentiometer;
   delete _cvIn;
}

void ControlPair::setPotentiometerScale(double scale) {
   _potentiometerScale = scale;
}

void ControlPair::setCvScale(double scale) {
   _cvScale = scale;
}

double ControlPair::update(double time) {
   double potIn = _potentiometer->read(time) * _potentiometerScale;
   double cvIn = _cvIn->read(time) * _cvScale;
   return potIn + cvIn;
}
