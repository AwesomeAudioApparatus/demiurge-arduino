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

#include <esp_system.h>
#include <string.h>
#include "driver/spi_master.h"

#include "MCP4822.h"

MCP4822::MCP4822(spi_device_handle_t spi) {
   _spi = spi;
   memset(&_tx1, 0, sizeof(_tx1));       //Zero out the transaction
   memset(&_tx2, 0, sizeof(_tx2));       //Zero out the transaction
   _tx1.flags = SPI_TRANS_USE_TXDATA;
   _tx1.length = 16;
   _tx2.flags = SPI_TRANS_USE_TXDATA;
   _tx2.length = 16;
}

esp_err_t MCP4822::dac1Output(uint16_t output) {
   _tx1.tx_data[0] = MCP4822_CHANNEL_A | MCP4822_GAIN | MCP4822_ACTIVE | ((output >> 8) & 0xFF);
   _tx1.tx_data[1] = output & 0xFF;
   return spi_device_queue_trans(_spi, &_tx1, 10); // 10 ticks is probably very long.
}

esp_err_t MCP4822::dac2Output(uint16_t output) {
   _tx2.tx_data[0] = MCP4822_CHANNEL_B | MCP4822_GAIN | MCP4822_ACTIVE | ((output >> 8) & 0xFF);
   _tx2.tx_data[1] = output & 0xFF;
   return spi_device_queue_trans(_spi, &_tx2, 10); // 10 ticks is probably very long.
}
