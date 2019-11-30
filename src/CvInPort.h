#ifndef _DEMIURGE_CVPORT_H_
#define _DEMIURGE_CVPORT_H_

#ifndef DEMIURGE_CVINPUT_OFFSET
#define DEMIURGE_CVINPUT_OFFSET -1
#endif

#include "Signal.h"

class CvInPort : public Signal {

public:
   explicit CvInPort(int position);
   ~CvInPort();

   int _position;

   double update(double time) override;
};


#endif
