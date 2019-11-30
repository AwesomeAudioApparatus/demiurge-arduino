#ifndef _DEMIURGE_CVPORT_H_
#define _DEMIURGE_CVPORT_H_

#include "Signal.h"

class CvInPort : public Signal {

public:
   CvInPort( int position );
   virtual ~CvInPort();

   double output();
};


#endif
