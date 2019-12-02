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
#include "Arduino.h"


ADC128S102::ADC128S102(spi_device_handle_t spi) {
   _spi = spi;
   memset(&_tx, 0, sizeof(_tx));       //Zero out the transaction
   _tx.length = 128;
   _tx.rxlength = 128;
   _tx.tx_buffer = &_txdata;
   _tx.rx_buffer = &_rxdata;
   _rx = nullptr;
   for (int i = 0; i < 7; i++) {
      _txdata[2*i+1] = 0;
      _txdata[2*i] = ((i+1) << 3);
   }
}

ADC128S102::~ADC128S102() {
}

void IRAM_ATTR ADC128S102::queue() {
   esp_err_t err = spi_device_queue_trans(_spi, &_tx, 10); // 10 ticks is probably very long.
   ESP_ERROR_CHECK(err)
}

void IRAM_ATTR ADC128S102::read() {
   esp_err_t err = spi_device_get_trans_result(_spi, &_rx, 10);
   ESP_ERROR_CHECK(err)
   for (int i = 0; i < 8; i++) {
      uint16_t high = _rxdata[i * 2];
      uint16_t low = _rxdata[i * 2 + 1];
      _channels[i] = low + (high << 8);
   }
}

uint16_t *ADC128S102::channels() {
   return _channels;
}



