
#ifndef DEMIURGE_OCTAVEPERVOLT_H
#define DEMIURGE_OCTAVEPERVOLT_H

#include <math.h>

class OctavePerVolt {

public:
   static double frequencyOf(double voltage) {
      return 440 / pow(2, 2.75) * pow(2, voltage);
   }

private:
   OctavePerVolt();

   ~OctavePerVolt();
};


#endif
