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

#ifndef _DEMIURGE_CVOUTPORT_H_
#define _DEMIURGE_CVOUTPORT_H_

#include "Signal.h"

typedef struct {
   int position;
   signal_t *me;
   signal_t *input;
   bool registered;
} cv_out_port_t;

float cvoutport_read(signal_t *handle, uint64_t time);

class CvOutPort : public Signal{

public:
   explicit CvOutPort(int position);
   ~CvOutPort() override;

   void configure( Signal* input );

   cv_out_port_t _data{};
private:
   Signal *_input{};
};


#endif
