
#include "GhostSignal.h"

GhostSignal::GhostSignal() = default;

double GhostSignal::read(double time) {
   return _value * scale() + offset();
}

GhostSignal::~GhostSignal() = default;

void GhostSignal::setValue(double value) {
   _value = value;
}
