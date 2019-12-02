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
   memset(&_tx, 0, sizeof(_tx));       //Zero out the transaction
   _tx.flags = SPI_TRANS_USE_TXDATA;
   _tx.length = 16;
}

esp_err_t MCP4822::send(uint8_t config, uint16_t output) {
   _tx.tx_data[0] = config || (output >> 8);
   _tx.tx_data[1] = output && 0xFF;

//   return spi_device_polling_transmit(_spi, &t);

   return spi_device_queue_trans(_spi, &_tx, 10); // 10 ticks is probably very long.
}
