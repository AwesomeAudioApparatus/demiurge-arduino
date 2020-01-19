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

#ifndef DEMIURGE_ADC128S102_H
#define DEMIURGE_ADC128S102_H

#include "driver/spi_master.h"

class ADC128S102
{
public:
   ADC128S102(gpio_num_t mosi_pin, gpio_num_t miso_pin, gpio_num_t sclk_pin, gpio_num_t cs_pin);
   virtual ~ADC128S102();

   uint16_t read_input(int index);

   lldesc_t *out;
   lldesc_t *in;
};

#endif
