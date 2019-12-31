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

#include <string.h>
#include "ADC128S102.h"


void adc128s102_init(adc128s102 *handle, spi_device_handle_t spi)
{
   handle->_spi = spi;
   memset(&handle->_tx, 0, sizeof(handle->_tx));       //Zero out the transaction

   handle->_txdata = static_cast<uint8_t *>(heap_caps_malloc(8, MALLOC_CAP_DMA));
   handle->_rxdata= static_cast<uint8_t *>(heap_caps_malloc(8, MALLOC_CAP_DMA));

   handle->_tx.length = 128;
   handle->_tx.rxlength = 0;
   handle->_tx.tx_buffer = handle->_txdata;
   handle->_tx.rx_buffer = handle->_rxdata;
   handle->_rx = nullptr;
   for (int i = 0; i < 7; i++) {
      handle->_txdata[2*i+1] = 0;
      handle->_txdata[2*i] = ((i+1) << 3);
   }
   //start endless transfer.
   esp_err_t err = spi_device_queue_trans(handle->_spi, &handle->_tx, 10); // 10 ticks is probably very long.
}

void adc128s102_sync_read(adc128s102 *handle){
//   gpio_set_level(GPIO_NUM_25, 0);
//   esp_err_t err = spi_device_polling_transmit(handle->_spi, &handle->_tx); // 10 ticks is probably very long.
//   ESP_ERROR_CHECK(err)

   for (int i = 0; i < 8; i++) {
      uint8_t *d = handle->_rxdata;
      uint16_t high = d[i * 2];
      uint16_t low = d[i * 2 + 1];
      handle->_channels[i] = low + (high << 8);
   }
//   gpio_set_level(GPIO_NUM_25, 1);
}

void adc128s102_queue(adc128s102 *handle)
{
//   gpio_set_level(GPIO_NUM_25, 0);
//   esp_err_t err = spi_device_queue_trans(handle->_spi, &handle->_tx, 10); // 10 ticks is probably very long.
//   ESP_ERROR_CHECK(err)
//   gpio_set_level(GPIO_NUM_25, 1);
}

void adc128s102_read(adc128s102 *handle)
{
//   gpio_set_level(GPIO_NUM_25, 0);
//   esp_err_t err = spi_device_get_trans_result(handle->_spi, &handle->_rx, 10);
//   ESP_ERROR_CHECK(err)
   for (int i = 0; i < 8; i++) {
      uint8_t *d = handle->_rxdata;
      uint16_t high = d[i * 2];
      uint16_t low = d[i * 2 + 1];
      handle->_channels[i] = low + (high << 8);
   }
//   gpio_set_level(GPIO_NUM_25, 1);
}
