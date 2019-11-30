
#ifndef _DEMIURGE_POTENTIOMETER_H_
#define _DEMIURGE_POTENTIOMETER_H_

#include "Signal.h"

class Potentiometer : public Signal {

public:
   Potentiometer(int position);
   virtual ~Potentiometer();

   double output();

};


#endif
