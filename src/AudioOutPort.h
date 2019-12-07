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

#ifndef _DEMIURGE_AUDIOOUTPORT_H_
#define _DEMIURGE_AUDIOOUTPORT_H_

#include "Signal.h"
#include "Sink.h"


class AudioOutPort : public Sink, public Signal{

public:
   explicit AudioOutPort(int position);
   ~AudioOutPort() override;

   void configure( Signal* input );

   void configure( Signal* input, float scale, float offset );

   void tick(uint64_t time) override;

protected:
   float update(uint64_t time) override;

private:
   int _position;
   bool _registered;
   Signal *_input;
};


#endif
