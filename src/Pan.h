
#ifndef _DEMIURGE_PAN_H_
#define _DEMIURGE_PAN_H_

#include "Signal.h"

class PanChannel;

class Pan {

public:
   Pan();

   void configure(Signal *input, Signal *control);

   Signal *outputLeft();

   Signal *outputRight();

private :
   friend class PanChannel;
   Signal *_input;
   Signal *_control;
   PanChannel *_left;
   PanChannel *_right;
};

class PanChannel : public Signal {

public:
   PanChannel(Pan *host, double factor);

   ~PanChannel() override;

   double update(double time) override;

private:
   Pan *_host;
   double _factor;
};

#endif
