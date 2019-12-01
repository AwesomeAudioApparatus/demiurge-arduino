
#include "Demiurge.h"

AudioOutPort::AudioOutPort(int position) {
   configASSERT(position > 0 && position <= 2 )
   _position = position;
   Demiurge::runtime().registerSink(this);
}

AudioOutPort::~AudioOutPort(){
   Demiurge::runtime().unregisterSink(this);
};

void AudioOutPort::configure(Signal *input) {
   _input = input;
}

void AudioOutPort::configure(Signal *input, double scale, double offset) {
   _input = input;
   setScale(scale);
   setOffset(offset);
}

double AudioOutPort::update(double time) {
   return _input->read(time);
}

void AudioOutPort::tick(double time) {
   double voltage = update(time);
   Demiurge::runtime().setDAC(_position, voltage);
}