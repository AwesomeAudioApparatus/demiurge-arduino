
#include <math.h>
#include <esp_task.h>
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
   configASSERT(freqControl != nullptr)
   _frequencyControl = freqControl;
}

void VCO::configureAmplitude(Signal *amplitudeControl) {
   configASSERT(amplitudeControl != nullptr)
   _amplitudeControl = amplitudeControl;
}

void VCO::configureTrig(Signal *trigControl) {
   configASSERT(trigControl != nullptr)
   _triggerControl = trigControl;
}

double VCO::update(double time) {
   double freq;
   if (_frequencyControl == nullptr)
      freq = 440;
   else
      freq = OctavePerVolt::frequencyOf(_frequencyControl->read(time));

   double amplitude;
   if (_amplitudeControl == nullptr)
      amplitude = 1.0;
   else
      amplitude = _amplitudeControl->read(time);

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
