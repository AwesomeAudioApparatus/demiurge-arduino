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

class ADC128S102 {

public:
   explicit ADC128S102(spi_device_handle_t spi);

   ~ADC128S102();

   void queue();

   void read();

   uint16_t *channels();

private :
   spi_device_handle_t _spi;
   spi_transaction_t _tx;
   spi_transaction_t *_rx;

   uint8_t _txdata[16];
   uint8_t _rxdata[16];

   uint16_t _channels[8];

};


#endif
