
#ifndef _DEMIURGE_POTENTIOMETER_H_
#define _DEMIURGE_POTENTIOMETER_H_

#ifndef DEMIURGE_CVINPUT_OFFSET
#define DEMIEURGE_POTENTIOMETER_OFFSET 4
#endif

#include "Signal.h"

class Potentiometer : public Signal {

public:
   explicit Potentiometer(int position);

   double update(double time) override;

private:
   int _position;
};


#endif
