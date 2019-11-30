
#ifndef _DEMIURGE_FIXEDSIGNAL_H_
#define _DEMIURGE_FIXEDSIGNAL_H_


#include "Signal.h"

class FixedSignal : public Signal {

public:
   explicit FixedSignal(double value);
   ~FixedSignal() override;

protected:
   double update(double time) override;

private:
   double _value;
};


#endif
