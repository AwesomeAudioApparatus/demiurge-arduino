#ifndef _DEMIURGE_THRESHOLD_H_
#define _DEMIURGE_THRESHOLD_H_


class Threshold {

public:
   Threshold();

   Threshold(double setpoint, double hyst);

   bool compute(double input);

private:
   double _setpoint;
   double _hysteresis;
   bool _output;
};


#endif
