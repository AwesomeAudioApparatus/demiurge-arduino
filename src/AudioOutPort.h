#ifndef _DEMIURGE_AUDIOOUTPORT_H_
#define _DEMIURGE_AUDIOOUTPORT_H_

#include "Signal.h"

class AudioOutPort : public Signal {

public:
   AudioOutPort(int position);
   virtual ~AudioOutPort();

   void configure( Signal* input );
   void configure( Signal* input, double scale, double offset );

};


#endif
