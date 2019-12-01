
#ifndef _DEMIURGE_PASSTHRU_H
#define _DEMIURGE_PASSTHRU_H


#include "Signal.h"

class Passthru : public Signal {

public:
   explicit Passthru(Signal *input);
   ~Passthru() override;

protected:
   double update(double time) override;

private:
   Signal *_input;
};


#endif
