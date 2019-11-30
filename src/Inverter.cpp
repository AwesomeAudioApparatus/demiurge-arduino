//
// Created by niclas on 11/30/19.
//

#include "Inverter.h"

Inverter::Inverter() {
   _midpoint = 0.0;
   _scale = 1.0;
};

Inverter::Inverter(double midpoint) {
   _midpoint = midpoint;
   _scale = 1.0;
}

Inverter::Inverter(double midpoint, double scale) {
   _midpoint = midpoint;
   _scale = scale;
}

void Inverter::configure(Signal *input) {
   _input = input;
}
