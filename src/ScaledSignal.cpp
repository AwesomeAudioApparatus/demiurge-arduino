
#include "ScaledSignal.h"

ScaledSignal::ScaledSignal(Signal *wrapped, double scale, double offset) {
   _wrapped = wrapped;
   setScale(scale);
   setOffset(offset);
}

double ScaledSignal::read(double time) {
   return _wrapped->read(time) * scale() + offset();
}
