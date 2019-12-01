/*
  Copyright 2019, Awesome Audio Apparatus.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

      https://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
      limitations under the License.
*/

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
