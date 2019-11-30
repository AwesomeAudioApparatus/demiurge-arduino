
#include "Signal.h"

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

double Signal::read(double time) {
   if (time > lastRead) {
      output = update(time);
   }
   return output;
}
