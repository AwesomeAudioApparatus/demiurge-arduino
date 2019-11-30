
#include "Demiurge.h"

AudioInPort::AudioInPort(int position) {
   _position = position;
}

void AudioInPort::configure(double scale, double offset) {
   setScale(scale);
   setOffset(offset);
}

double AudioInPort::read(double time) {
   return scale() * Demiurge::runtime()->inputs()->value[_position] + offset();
}
