
#include "Signal.h"

Signal::~Signal() {}

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
   if (time > _lastRead)
   {
      _output = update(time);// * _scale + _offset;
      _lastRead = time;
   }
   return _output;
}

double Signal::currentValue() {
   return _output;
}
