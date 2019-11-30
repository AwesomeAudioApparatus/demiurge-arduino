
#ifndef _DEMIURGE_VCO_H_
#define _DEMIURGE_VCO_H_


#include "Signal.h"

class VCO : Signal {

public:
   VCO();

   virtual ~VCO();

protected:
   void configureFrequency(Signal *freqControl);

   void configureAmplitude(Signal *amplitudeControl);

   void configureTrig(Signal *trigControl);

   void configureMute(Signal *muteControl);
};

#endif
