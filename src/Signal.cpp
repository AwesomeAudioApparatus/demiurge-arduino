
#include "Signal.h"

double Signal::output() {
   return _output;
}

double Signal::scale() {
   return _scale;
}

double Signal::offset() {
   return _offset;
}

void Signal::setScale(double scale) {
   _scale = scale;
}

void Signal::setOffset(double offset) {
   _offset = offset;
}
