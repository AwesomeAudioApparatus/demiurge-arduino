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
#include <esp_log.h>
#include "ADC128S102.h"
#include "../spi/aaa_spi.h"

#define TAG "ADC128S102"

ADC128S102::ADC128S102(gpio_num_t mosi_pin, gpio_num_t miso_pin, gpio_num_t sclk_pin, gpio_num_t cs_pin)
{
   ESP_LOGE(TAG, "Initializing SPI.");

   out = static_cast<lldesc_t *>(heap_caps_malloc(sizeof(lldesc_t), MALLOC_CAP_DMA));

   memset((void *) out, 0, sizeof(lldesc_t));
   out->size = 16;
   out->length = 16;
   out->offset = 0;
   out->sosf = 0;
   out->eof = 0;
   out->owner = 1;
   out->qe.stqe_next = out;
   out->buf = static_cast<uint8_t *>(heap_caps_malloc(16, MALLOC_CAP_DMA));
   out->buf[0] = 1 << 3;
   out->buf[1] = 0;
   out->buf[2] = 2 << 3;
   out->buf[3] = 0;
   out->buf[4] = 3 << 3;
   out->buf[5] = 0;
   out->buf[6] = 4 << 3;
   out->buf[7] = 0;
   out->buf[8] = 5 << 3;
   out->buf[9] = 0;
   out->buf[10] = 6 << 3;
   out->buf[11] = 0;
   out->buf[12] = 7 << 3;
   out->buf[13] = 0;
   out->buf[14] = 0 << 3;
   out->buf[15] = 0;

   in = static_cast<lldesc_t *>(heap_caps_malloc(sizeof(lldesc_t), MALLOC_CAP_DMA));
   memset((void *) in, 0, sizeof(lldesc_t));
   in->buf = static_cast<uint8_t *>(heap_caps_malloc(16, MALLOC_CAP_DMA));
   in->size = 16;
   in->length = 16;
   in->offset = 0;
   in->sosf = 0;
   in->eof = 0;
   in->owner = 1;
   in->qe.stqe_next = in;
   in->buf = static_cast<uint8_t *>(heap_caps_malloc(16, MALLOC_CAP_DMA));

   esp_err_t error = aaa_spi_prepare_circular(VSPI_HOST, 2, out, in, 1500000, mosi_pin, miso_pin, sclk_pin, 0);
   ESP_ERROR_CHECK(error)

   gpio_set_direction(cs_pin, GPIO_MODE_OUTPUT);
   gpio_set_level(cs_pin, 0);
   spi_dev_t *const spiHw = aaa_spi_get_hw_for_host(VSPI_HOST);
   spiHw->dma_in_link.start = 1;   // Start SPI DMA transfer (1)  M
   spiHw->dma_out_link.start = 1;   // Start SPI DMA transfer (1)  M
   spiHw->cmd.usr = 1;   // SPI: Start SPI DMA transfer
}

ADC128S102::~ADC128S102()
{
   // Stop hardware
   spi_dev_t *const spiHw = aaa_spi_get_hw_for_host(VSPI_HOST);
   spiHw->cmd.usr = 0;   // SPI: Stop SPI DMA transfer
   aaa_spi_release_circular_buffer(VSPI_HOST, 1);
   free(out);
   free(in);
}

uint16_t ADC128S102::read_input(int index)
{
   return (in->buf[2*index] << 8) + in->buf[2*index + 1];
}

#undef TAG