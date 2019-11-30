#ifndef _DEMIURGE_SIGNAL_H_
#define _DEMIURGE_SIGNAL_H_


#include "SoundProcessor.h"

class Signal : public SoundProcessor {

public:
   virtual ~Signal() = 0;
   double scale();
   double offset();

   void setScale( double scale );
   void setOffset( double offset );

   double read(double time) final;

protected:
   virtual double update(double time);

private:
   double lastRead = -1.0;
   double output = 0.0;
   double _scale = 1.0;
   double _offset = 0.0;
};

#endif
