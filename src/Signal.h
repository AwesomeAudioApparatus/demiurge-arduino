#ifndef DEMIURGE_SIGNAL_H
#define DEMIURGE_SIGNAL_H


#include "SoundProcessor.h"

class Signal : SoundProcessor {

public:
   double output();

   double scale();
   double offset();

   void setScale( double scale );
   void setOffset( double offset );

private:
   double _output;
   double _input;
   double _scale;
   double _offset;
};


#endif
