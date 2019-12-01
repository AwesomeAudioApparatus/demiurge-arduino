#ifndef _DEMIURGE_AUDIOOUTPORT_H_
#define _DEMIURGE_AUDIOOUTPORT_H_

#include "Signal.h"
#include "Sink.h"


class AudioOutPort : public Sink, public Signal{

public:
   explicit AudioOutPort(int position);
   ~AudioOutPort() override;

   void configure( Signal* input );

   void configure( Signal* input, double scale, double offset );

   void tick(double time) override;

protected:
   double update(double time) override;

private:
   int _position;
   Signal *_input;
};


#endif
