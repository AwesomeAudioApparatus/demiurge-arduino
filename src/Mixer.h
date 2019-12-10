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

#ifndef _DEMIURGE_MIXER_H_
#define _DEMIURGE_MIXER_H_

#define DEMIURGE_MAX_MIXER_IN 4
#include <list>
#include "Signal.h"
#include "Passthru.h"
#include "Volume.h"

typedef struct {
   signal_t *me;
   signal_t *inputs[DEMIURGE_MAX_MIXER_IN];
} mixer_t;

float mixer_read(void *handle, uint64_t time);


class Mixer : public Signal {

public:
   explicit Mixer();

   ~Mixer() override;

   void configure(int number, Signal *source, Signal *control);

private:
   mixer_t _data{};
};


#endif
