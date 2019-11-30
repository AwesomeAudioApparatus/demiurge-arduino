
#include "Demiurge.h"
#include "Pan.h"


Pan::Pan() {
   _left = new PanChannel(this, 0.05);
   _right = new PanChannel(this, -0.05);
};

void Pan::configure(Signal *input, Signal *control) {
   _input = input;
   _control = control;
}

Signal *Pan::outputLeft() {
   return _left;
}

Signal *Pan::outputRight() {
   return _right;
}

PanChannel::PanChannel(Pan *host, double factor) {
   _host = host;
   _factor = factor;
}

PanChannel::~PanChannel() = default;

double PanChannel::update(double time) {
   double control = _host->_control->read(time);
   return _host->_input->read(time) * control * _factor;
}
