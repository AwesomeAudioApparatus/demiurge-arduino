
#include "Demiurge.h"
#include "AudioInPort.h"


AudioInPort::AudioInPort(int position) {
   _position = position;
}

AudioInPort::~AudioInPort() = default;

void AudioInPort::configure(double scale, double offset) {
   setScale(scale);
   setOffset(offset);
}

double AudioInPort::update(double time) {
   return scale() * Demiurge::runtime()->inputs()->value[_position] + offset();
}
