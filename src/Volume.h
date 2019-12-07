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

#ifndef DEMIURGE_VOLUME_H
#define DEMIURGE_VOLUME_H


#include "Signal.h"

class Volume : public Signal {

public:
   Volume();

   ~Volume() override;

   void configure( Signal *input, Signal *control);

protected:
   float update(uint64_t time) override;

private:
   Signal *_input;
   Signal *_control;
};


#endif
