
#include "Demiurge.h"

AudioOutPort::AudioOutPort(int position) {
   _position = position;
}

AudioOutPort::~AudioOutPort() = default;

void AudioOutPort::configure(Signal *input) {
   _input = input;
}

void AudioOutPort::configure(Signal *input, double scale, double offset) {
   _input = input;
   setScale(scale);
   setOffset(offset);
}

void AudioOutPort::tick(double time) {
   double voltage = scale() * _input->read(time) + offset();
   Demiurge::runtime()->setDAC(_position, voltage);
}