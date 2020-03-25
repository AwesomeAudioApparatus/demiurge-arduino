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
#include <driver/periph_ctrl.h>
#include <soc/mcpwm_struct.h>
#include <soc/mcpwm_reg.h>
#include <driver/mcpwm.h>
#include <soc/spi_struct.h>

#include "ADC128S102.h"
#include "../spi/aaa_spi.h"

#define TAG "ADC128S102"

static void initialize(gpio_num_t pin_out) {
   ESP_LOGI(TAG, "Initializing ADC timer.");

   // We use MCPWM0 TIMER 1 Operator 1 for CS generation to DAC.

   PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[pin_out], PIN_FUNC_GPIO);
   gpio_set_direction(pin_out, GPIO_MODE_INPUT_OUTPUT);
   gpio_matrix_out(pin_out, PWM0_OUT1A_IDX, false, false);  // PWM0 operator 1 output A
   periph_module_enable(PERIPH_PWM0_MODULE);

   WRITE_PERI_REG(MCPWM_TIMER_SYNCI_CFG_REG(0), (1 << MCPWM_TIMER1_SYNCISEL_S));
   WRITE_PERI_REG(MCPWM_OPERATOR_TIMERSEL_REG(0), (1 << MCPWM_OPERATOR1_TIMERSEL_S));

   WRITE_PERI_REG(MCPWM_GEN1_TSTMP_A_REG(0), 512);  // 128 cycles LOW after UTEZ.
   WRITE_PERI_REG(MCPWM_GEN1_A_REG(0),
                  (1 << MCPWM_GEN1_A_UTEZ_S) | (2 << MCPWM_GEN1_A_UTEA_S)); // UTEZ= set PWM0A low, UTEA=set PWM0A high

   WRITE_PERI_REG(MCPWM_TIMER1_CFG0_REG(0), 3 << MCPWM_TIMER1_PRESCALE_S | 639
         << MCPWM_TIMER1_PERIOD_S);    // Prescale=16, so timer is 10MHz, 40 clocks per cycle
   WRITE_PERI_REG(MCPWM_TIMER1_CFG1_REG(0), (1 << MCPWM_TIMER1_MOD_S) |
                                            (2 << MCPWM_TIMER1_START_S));        // Continuously running, decrease mode.


   ESP_LOGI(TAG, "Initializing ADC timer....Done");
}

ADC128S102::ADC128S102(gpio_num_t mosi_pin, gpio_num_t miso_pin, gpio_num_t sclk_pin, gpio_num_t cs_pin) {
   ESP_LOGE(TAG, "Initializing ADC SPI.");

   out = static_cast<lldesc_t *>(heap_caps_malloc(sizeof(lldesc_t), MALLOC_CAP_DMA));

   memset((void *) out, 0, sizeof(lldesc_t));
   out->size = 20;
   out->length = 20;
   out->offset = 0;
   out->sosf = 0;
   out->eof = 0;
   out->qe.stqe_next = out;
   out->buf = static_cast<uint8_t *>(heap_caps_malloc(20, MALLOC_CAP_DMA));
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
   out->buf[16] = 255;
   out->buf[17] = 255;
   out->buf[18] = 255;
   out->buf[19] = 255;
   out->owner = 1;

   in = static_cast<lldesc_t *>(heap_caps_malloc(sizeof(lldesc_t), MALLOC_CAP_DMA));
   memset((void *) in, 0, sizeof(lldesc_t));
   in->size = 20;
   in->length = 20;
   in->offset = 0;
   in->sosf = 0;
   in->eof = 0;
   in->qe.stqe_next = in;
   in->buf = static_cast<uint8_t *>(heap_caps_malloc(32, MALLOC_CAP_DMA));
   memset((void *) in->buf, 0, 32);
   in->owner = 1;

   // Values to be written during time critical stage
   auto s1 = (1 << MCPWM_TIMER1_MOD_S) | (2 << MCPWM_TIMER1_START_S);
   auto s3 = (577 << MCPWM_TIMER1_PHASE_S) | (0 << MCPWM_TIMER1_SYNCO_SEL) | (1 << MCPWM_TIMER1_SYNC_SW_S);

   portDISABLE_INTERRUPTS();  // No interference in timing.
   initialize(cs_pin);

   esp_err_t error = aaa_spi_prepare_circular(VSPI_HOST, 2, out, in, 10000000, mosi_pin, miso_pin, sclk_pin, 0);
   ESP_ERROR_CHECK(error)

   spi_dev_t *const spiHw = aaa_spi_get_hw_for_host(VSPI_HOST);

   // this bit of code makes sure both timers and SPI transfer are started as close together as possible
   for (int i = 0; i < 2; i++) {  // Make sure SPI Flash fetches doesn't interfere

      //Reset DMA
      spiHw->dma_out_link.start = 0;
      spiHw->dma_in_link.start = 0;
      spiHw->cmd.usr = 0;   // SPI: Stop SPI DMA transfer

      // --- sync to known prescaled cycle.
      auto reg = READ_PERI_REG(MCPWM_TIMER1_STATUS_REG(0));
      while (reg == READ_PERI_REG(MCPWM_TIMER1_STATUS_REG(0)));

      spiHw->cmd.val = 0;
      spiHw->dma_conf.val |= SPI_OUT_RST | SPI_IN_RST | SPI_AHBM_RST | SPI_AHBM_FIFO_RST;
      spiHw->dma_conf.val &= ~(SPI_OUT_RST | SPI_IN_RST | SPI_AHBM_RST | SPI_AHBM_FIFO_RST);
      spiHw->dma_conf.out_data_burst_en = 0;

      spiHw->dma_out_link.start = 1;   // Start SPI DMA transfer (1)  M
      spiHw->dma_in_link.start = 1;   // Start SPI DMA transfer (1)  M
      WRITE_PERI_REG(MCPWM_TIMER1_CFG1_REG(0), s1); // start timer 1
      spiHw->cmd.usr = 1; // start SPI transfer

      WRITE_PERI_REG(MCPWM_TIMER1_SYNC_REG(0), s3);
   }

   portENABLE_INTERRUPTS();
   ESP_LOGI(TAG, "Initializing ADC SPI.....Done");

}

ADC128S102::~ADC128S102() {
   // Stop hardware
   WRITE_PERI_REG(MCPWM_TIMER1_CFG1_REG(0),
                  READ_PERI_REG(MCPWM_TIMER1_CFG1_REG(0)) & ~MCPWM_TIMER1_MOD_M); // stop timer 1

   aaa_spi_release_circular_buffer(VSPI_HOST, 1);
   free(out);
   free(in);
}


// There seems to be a Hardware bug that the continuous MISO reading in DMA causing
// the bytes to "shift" every 450 seconds or there about. So, to work around
// that, we look for three bytes with 255, as that is when the timer1 is producing
// CS HIGH and disabling the ADC, and the MISO will shift in a floating MISO (weakly pulled high)
// the first byte during CS HIGH will be during the rising of the level and is not
// 255 and varies.
void ADC128S102::copy_buffer(void *dest) {
   uint8_t volatile *buf = in->buf;

   if( buf[0] == 255 || buf[19] == 255)
   {
      // we have an unbroken middle section. Nice!
      if(buf[0] != 255 )
         memcpy(dest, (void *) buf, 16);
      else if(buf[1] != 255 )
         memcpy(dest, (void *) &buf[1], 16);
      else if(buf[2] != 255 )
         memcpy(dest, (void *) &buf[2], 16);
      else
         memcpy(dest, (void *) &buf[3], 16);
   } else {
      // we have to segments to be copied.
      for( int i=0; i < 20; i++){
         if( buf[i] == 255 && buf[i+1] == 255 ){
            // found the location

            // Copy the beginning, that comes after the three 0xFF bytes
            memcpy(dest, (void *) &buf[i+3], 17-i);

            // Copy the remaining from buffer, which sits at the beginning, into the end of the destination buffer.
            memcpy(((uint8_t *)dest)+17-i, (void *) buf, i-1);
         }
      }
   }
}
#undef TAG