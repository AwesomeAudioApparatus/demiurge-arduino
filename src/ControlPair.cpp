
#include "ControlPair.h"

ControlPair::ControlPair(int position) {
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
