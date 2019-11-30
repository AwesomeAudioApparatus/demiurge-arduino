#ifndef DEMIURGE_SIGNAL_H
#define DEMIURGE_SIGNAL_H


#include "SoundProcessor.h"

class Signal : public SoundProcessor {

public:
   virtual ~Signal() = 0;
   double scale();
   double offset();

   void setScale( double scale );
   void setOffset( double offset );

private:
   double _scale = 1.0;
   double _offset = 0.0;
};


#endif
