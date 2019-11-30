
#include "Threshold.h"

Threshold::Threshold() {
   _setpoint = 2.5;
   _hysteresis = 1.0;
   _output = false;
}

Threshold::Threshold(double setp, double hyst) {
   _setpoint = setp;
   _hysteresis = hyst;
   _output = false;
}

bool Threshold::compute(double input) {
   if (_output) {
      _output = input < _setpoint - _hysteresis;
   } else {
      _output = input > _setpoint + _hysteresis;
   }
   return _output;
}
