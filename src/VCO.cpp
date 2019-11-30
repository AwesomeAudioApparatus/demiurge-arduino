
#include <math.h>
#include "VCO.h"
#include "OctavePerVolt.h"

static const double SINE_CONST = 1000000 / M_TWOPI;

VCO::VCO(int mode) {
   _mode = mode;
   _triggerControl = nullptr;
   _amplitudeControl = nullptr;
   _frequencyControl = nullptr;
}

VCO::~VCO() = default;

void VCO::configureFrequency(Signal *freqControl) {
   _frequencyControl = freqControl;
}

void VCO::configureAmplitude(Signal *amplitudeControl) {
   _amplitudeControl = amplitudeControl;
}

void VCO::configureTrig(Signal *trigControl) {
   _triggerControl = trigControl;
}

double VCO::update(double time) {
   double freq = OctavePerVolt::frequencyOf(_frequencyControl->read(time));
   double amplitude = _amplitudeControl->read(time);

   switch (_mode) {
      case DEMIURGE_SINE:
         return sin(freq * (time - _lastTrig) / SINE_CONST) * amplitude;
      case DEMIURGE_SQUARE:
         // TODO
         break;
      case DEMIURGE_TRIANGLE:
         // TODO
         break;
      case DEMIURGE_SAW:
         // TODO
         break;
   }
}
