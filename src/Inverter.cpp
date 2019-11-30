
#include "Inverter.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#pragma ide diagnostic ignored "cppcoreguidelines-pro-type-member-init"

Inverter::Inverter() {
   _midpoint = 0.0;
   _scale = 1.0;
}

Inverter::Inverter(double midpoint) {
   _midpoint = midpoint;
   _scale = 1.0;
}

Inverter::Inverter(double midpoint, double scale) {
   _midpoint = midpoint;
   _scale = scale;
}

Inverter::~Inverter() = default;

void Inverter::configure(Signal *input) {
   _input = input;
}

double Inverter::read(double time) {
   return 0;
}

#pragma clang diagnostic pop
