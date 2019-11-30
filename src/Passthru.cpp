
#include "Passthru.h"

Passthru::Passthru(Signal *input) {
   _input = input;
}

Passthru::~Passthru() = default;

double Passthru::update(double time) {
   return _input->read(time) * scale() + offset();
}
