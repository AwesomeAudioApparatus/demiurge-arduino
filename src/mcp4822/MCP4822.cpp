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

void mcp4822_init( mcp4822 *handle, spi_device_handle_t spi) {
   spi_transaction_t *transaction = &handle->_tx;
   handle->_spi = spi;
   memset(transaction, 0, sizeof(spi_transaction_t));       //Zero out the transaction

   handle->_txdata = static_cast<uint8_t *>(heap_caps_malloc(16, MALLOC_CAP_DMA));
   handle->_rxdata= static_cast<uint8_t *>(heap_caps_malloc(16, MALLOC_CAP_DMA));

   transaction->length = 40;
   transaction->rxlength = 0;
   transaction->tx_buffer = handle->_txdata;
   transaction->rx_buffer = handle->_rxdata;

   uint16_t outputA = 0x800;     // 50%
   uint16_t outputB = 0x400;     // 25%

//   handle->_txdata[0] = MCP4822_CHANNEL_A | MCP4822_ACTIVE | ((outputA >> 8) & 0xFF);
//   handle->_txdata[1] = outputA & 0xFF;
//   handle->_txdata[2] = MCP4822_CHANNEL_B | MCP4822_ACTIVE | ((outputB >> 8) & 0xFF);
//   handle->_txdata[3] = outputB & 0xFF;
   handle->_txdata[0] = 0;
   handle->_txdata[1] = 0;
   handle->_txdata[2] = 0;
   handle->_txdata[3] = 0;
   handle->_txdata[4] = 0;
   handle->_txdata[5] = 0;
   handle->_txdata[6] = 0;
   handle->_txdata[7] = 0;
   handle->_txdata[8] = 0;
   handle->_txdata[9] = 0;
   handle->_txdata[10] = 0;
   handle->_txdata[11] = 0;
   handle->_txdata[12] = 0;
   handle->_txdata[13] = 0;
   handle->_txdata[14] = 0;
   handle->_txdata[15] = 0;

   esp_err_t error = spi_device_queue_trans(handle->_spi, transaction, 1);
   ESP_ERROR_CHECK(error)
}

void mcp4822_dacOutput(mcp4822 *handle, uint16_t outputA, uint16_t outputB)
{
//   gpio_set_level(GPIO_NUM_22, 0);
//   handle->_tx.tx_data[0] = MCP4822_CHANNEL_A | MCP4822_ACTIVE | ((outputA >> 8) & 0xFF);
//   handle->_tx.tx_data[1] = outputA & 0xFF;
//   handle->_tx.tx_data[2] = MCP4822_CHANNEL_B | MCP4822_ACTIVE | ((outputB >> 8) & 0xFF);
//   handle->_tx.tx_data[3] = outputB & 0xFF;
////   spi_device_queue_trans(handle->_spi, &handle->_tx, 1); // 10 ticks is probably very long.
//   spi_device_polling_transmit(handle->_spi, &handle->_tx);
//   gpio_set_level(GPIO_NUM_22, 1);
}
