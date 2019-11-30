
#include "FixedSignal.h"

FixedSignal::FixedSignal(double value) {
   _value = value;
}

FixedSignal::~FixedSignal() = default;

double FixedSignal::update(double time) {
   return _value;
}
