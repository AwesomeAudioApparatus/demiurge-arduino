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

#ifndef _DEMIURGE_PAN_H_
#define _DEMIURGE_PAN_H_

#include "Signal.h"

typedef struct {
   float factor;
   signal_t *me;
   signal_t *control;
   signal_t *hostInput;
   uint64_t lastCalc;
   float cached;
} panchannel_t;

float IRAM_ATTR panchannel_read(signal_t *handle, uint64_t time);

class PanChannel;

class Pan {

public:
   Pan();

   ~Pan();

   void configure(Signal *input, Signal *control);

   Signal *outputLeft();

   Signal *outputRight();

private :
   friend class PanChannel;

   Signal *_input{};
   Signal *_control{};
   PanChannel *_left;
   PanChannel *_right;
};

class PanChannel : public Signal {

public:
   PanChannel(Pan *host, float factor);

   ~PanChannel() override;

private:
   panchannel_t _data{};
   Pan *_host;
};

#endif
