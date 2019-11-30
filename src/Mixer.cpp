
#include <malloc.h>
#include "Mixer.h"

Mixer::Mixer(int numberOfInputs) {
   _inputs = static_cast<Volume **>(malloc(sizeof(Volume *) * numberOfInputs));
   _numberOfInputs = numberOfInputs;
}

Mixer::~Mixer() {
   for (int i = 0; i < _numberOfInputs; i++) {
      delete _inputs[i];
   }
   free(_inputs);
}

void Mixer::configure(int number, Signal *source, Signal *control) {
   if (number >= 0 && number < _numberOfInputs) {
      Volume *v = new Volume();
      v->configure(source, control);
      _inputs[number] = v;
   }
}

double Mixer::update(double time) {
   double output = 0.0;
   for (int i = 0; i < _numberOfInputs; i++) {
      output = output + _inputs[i]->read(time);
   }
   return output;
}
