#ifndef _DEMIURGE_CVPORT_H_
#define _DEMIURGE_CVPORT_H_

#include "Signal.h"

class CvInPort : Signal {

public:
   CvInPort( int position );
   virtual ~CvInPort();

   double output();
};


#endif
