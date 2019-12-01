
#ifndef _DEMIURGE_VCO_H_
#define _DEMIURGE_VCO_H_

#define DEMIURGE_SINE 1
#define  DEMIURGE_SQUARE 2
#define  DEMIURGE_TRIANGLE 3
#define  DEMIURGE_SAW 4

#include "Signal.h"

class VCO : public Signal {

public:
   VCO(int mode);

   ~VCO() ;

   void configureFrequency(Signal *freqControl);

   void configureAmplitude(Signal *amplitudeControl);

   void configureTrig(Signal *trigControl);

protected:
   double update(double time) override;

private:
   int _mode;
   double _lastTrig;
   Signal *_frequencyControl;
   Signal *_amplitudeControl;
   Signal *_triggerControl;
};

#endif
