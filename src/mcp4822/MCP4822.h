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

#ifndef _DEMIURGE_MCP4822_h_
#define _DEMIURGE_MCP4822_h_

#include "driver/spi_master.h"

#define MCP4822_CHANNEL_A 0x00
#define MCP4822_CHANNEL_B 0x80
#define MCP4822_GAIN 0x20
#define MCP4822_ACTIVE 0x10

class MCP4822 {
public:
   explicit MCP4822(spi_device_handle_t spi);

   esp_err_t dac1Output(uint16_t data);

   esp_err_t dac2Output(uint16_t data);

private:
   spi_device_handle_t _spi;
   spi_transaction_t _tx1;
   spi_transaction_t _tx2;
};

#endif