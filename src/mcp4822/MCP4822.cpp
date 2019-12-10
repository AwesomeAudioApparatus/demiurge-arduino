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

void mcp4822_dacOutput(mcp4822 *handle, uint16_t outputA, uint16_t outputB)
{
   handle->_tx.tx_data[0] = MCP4822_CHANNEL_A | MCP4822_ACTIVE | ((outputA >> 8) & 0xFF);
   handle->_tx.tx_data[1] = outputA & 0xFF;
   handle->_tx.tx_data[2] = MCP4822_CHANNEL_B | MCP4822_ACTIVE | ((outputB >> 8) & 0xFF);
   handle->_tx.tx_data[3] = outputB & 0xFF;
   spi_device_queue_trans(handle->_spi, &handle->_tx, 10); // 10 ticks is probably very long.
}

void mcp4822_init( mcp4822 *handle, spi_device_handle_t spi) {
   handle->_spi = spi;
   memset(&handle->_tx, 0, sizeof(handle->_tx));       //Zero out the transaction
   handle->_tx.flags = SPI_TRANS_USE_TXDATA;
   handle->_tx.length = 32;
}
