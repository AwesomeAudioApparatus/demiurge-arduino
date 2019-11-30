#ifndef _DEMIURGE_DEMIURGE_H_
#define _DEMIURGE_DEMIURGE_H_


#include "Adsr.h"
#include "AudioInPort.h"
#include "AudioOutPort.h"
#include "CvInPort.h"
#include "ControlPair.h"
#include "GatePort.h"
#include "Inverter.h"
#include "Mixer.h"
#include "Pan.h"
#include "Potentiometer.h"
#include "PushButton.h"
#include "Signal.h"
#include "SoundProcessor.h"
#include "Threshold.h"
#include "VCO.h"

class Demiurge {

public:
   static void initialize(int sampleRate)
   {
      if( _initialized )
         return;
      _runtime = new Demiurge(sampleRate);
      _initialized = true;
   }

   static void shutdown()
   {
      delete _runtime;
      _initialized = false;
   }

   static Demiurge* runtime()
   {
      return _runtime;
   }

private:
   Demiurge(long sampleRate);
   ~Demiurge();

   static bool _initialized;
   static Demiurge* _runtime;
};

#endif
