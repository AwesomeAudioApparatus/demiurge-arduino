
#include <esp_task.h>
#include "Demiurge.h"

Volume::Volume() {
}

Volume::~Volume() {

}

void Volume::configure(Signal *input, Signal *control) {
   configASSERT(input != nullptr && control != nullptr)
   _input = input;
   _control = control;
}

double Volume::update(double time) {
   double in = _input->read(time);
   double control = _control->read(time);
   double factor = 0.05 * control + 0.5;
   return in * factor;
}
