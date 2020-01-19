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

#include <stdint.h>

typedef struct {
   int32_t  setpoint = 0x2000;   // ~2.5V
   int32_t  hysteresis = 0x800;  // ~0.625V
   bool output = false;
} threshold_t;

void threshold_init(threshold_t *data, int32_t  setp, int32_t  hyst);

bool threshold_compute(threshold_t *data, int32_t  input);


#endif
