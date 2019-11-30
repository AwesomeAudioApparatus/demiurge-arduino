
#include <malloc.h>
#include "Mixer.h"

Mixer::Mixer(int numberOfInputs) {
   _inputs = static_cast<ScaledSignal **>(malloc(sizeof(ScaledSignal *) * numberOfInputs));
   _volumes = static_cast<Signal **>(malloc(sizeof(Signal *) * numberOfInputs));
   _numberOfInputs = numberOfInputs;
}

Mixer::~Mixer() {
   for (int i = 0; i < _numberOfInputs; i++) {
      delete _volumes[i];
   }
   free(_inputs);
   free(_volumes);
}

void Mixer::configureInput(int number, Signal *source, double scale, double offset) {
   if (number >= 0 && number < _numberOfInputs) {
      _inputs[number] = new ScaledSignal(source, scale, offset);
   }
}

void Mixer::configureVolume(int number, Signal *source, double scale, double offset) {
   if (number >= 0 && number < _numberOfInputs) {
      _volumes[number] = new ScaledSignal(source, scale, offset);
   }
}

