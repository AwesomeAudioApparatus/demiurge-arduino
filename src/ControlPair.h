
#ifndef _DEMIURGE_CONTROLPAIR_H_
#define _DEMIURGE_CONTROLPAIR_H_

#include "Signal.h"
#include "Potentiometer.h"
#include "CvInPort.h"

class ControlPair : public Signal {

public:
   ControlPair(int position);

   ~ControlPair();

   void setPotentiometerScale(double scale);

   void setCvScale(double scale);

private:
   Potentiometer *_potentiometer;
   CvInPort *_cvIn;
   double _cvScale;
   double _potentiometerScale;
};


#endif
