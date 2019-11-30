
#ifndef _DEMIURGE_SOUNDPROCESSOR_H_
#define _DEMIURGE_SOUNDPROCESSOR_H_


class SoundProcessor {

public:
   virtual void readInputs();
   virtual void compute();
   virtual void writeOutputs();
};


#endif
