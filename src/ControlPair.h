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

#ifndef _DEMIURGE_CONTROLPAIR_H_
#define _DEMIURGE_CONTROLPAIR_H_

#include "Signal.h"
#include "Potentiometer.h"
#include "CvInPort.h"

typedef struct {
   signal_t *cv;
   signal_t *potentiometer;
   uint64_t lastCalc;
   int32_t cached;
} control_pair_t;

int32_t controlpair_read(signal_t *handle, uint64_t time);

class ControlPair : public Signal {

public:
   explicit ControlPair(int position);

   ~ControlPair() override;

   void setPotentiometerScale(int32_t scale);

   void setCvScale(int32_t scale);

private:
   control_pair_t _data;
   Potentiometer *_potentiometer;
   CvInPort *_cvIn;
   int32_t _cvScale;
   int32_t _potentiometerScale;
};


#endif
