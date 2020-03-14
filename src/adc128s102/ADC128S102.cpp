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

#include "ADC128S102.h"
#include "../spi/aaa_spi.h"

#define TAG "ADC128S102"

static void initialize(gpio_num_t pin_out) {
   ESP_LOGI(TAG, "Initializing DAC timer.");

   // We use MCPWM0 TIMER 1 for CS generation to ADC.

   PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[pin_out], PIN_FUNC_GPIO);
   gpio_set_direction(pin_out, GPIO_MODE_INPUT_OUTPUT);
   gpio_matrix_out(pin_out, PWM0_OUT0B_IDX, false, false);
   periph_module_enable(PERIPH_PWM0_MODULE);

   WRITE_PERI_REG(MCPWM_TIMER_SYNCI_CFG_REG(0), (1 << MCPWM_TIMER1_SYNCISEL_S));
   WRITE_PERI_REG(MCPWM_OPERATOR_TIMERSEL_REG(0), (1 << MCPWM_OPERATOR1_TIMERSEL_S));

   WRITE_PERI_REG(MCPWM_GEN1_TSTMP_B_REG(0), 16);  // 32 cycles LOW after UTEZ.
   WRITE_PERI_REG(MCPWM_GEN1_B_REG(0),
                  (1 << MCPWM_GEN0_B_UTEZ_S) | (2 << MCPWM_GEN0_B_UTEA_S)); // UTEZ= set PWM0B low, UTEA=set PWM0B high

   WRITE_PERI_REG(MCPWM_TIMER1_CFG0_REG(0), 15 << MCPWM_TIMER1_PRESCALE_S | 23 << MCPWM_TIMER1_PERIOD_S);    // Prescale=16, so timer is 10MHz, 40 clocks per cycle
   WRITE_PERI_REG(MCPWM_TIMER1_CFG1_REG(0), (1 << MCPWM_TIMER1_MOD_S) |
                                            (2 << MCPWM_TIMER1_START_S));        // Continuously running, decrease mode.


   ESP_LOGI(TAG, "Initializing ADC timer....Done");
}

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

   // Values to be written during time critical stage
   auto s0 = 1 << SPI_USR_S;
   auto s1 = (1 << MCPWM_TIMER1_MOD_S) | (2 << MCPWM_TIMER1_START_S);
   auto s3 = (9 << MCPWM_TIMER1_PHASE_S) | (0 << MCPWM_TIMER1_SYNCO_SEL) | (1 << MCPWM_TIMER1_SYNC_SW_S);

   // this bit of code makes sure both timers and SPI transfer are started as close together as possible
   for (int i = 0; i < 2; i++) {  // Make sure SPI Flash fetches doesn't interfere

      portDISABLE_INTERRUPTS();  // No interference in timing.
      spiHw->dma_in_link.start = 0;   // Stop SPI DMA transfer (1)
      spiHw->dma_out_link.start = 0;   // Stop SPI DMA transfer (1)
      spiHw->cmd.usr = 0;   // SPI: Stop SPI DMA transfer

      // --- sync to known prescaled cycle.
      auto reg = READ_PERI_REG(MCPWM_TIMER0_STATUS_REG(0));
      while (reg == READ_PERI_REG(MCPWM_TIMER0_STATUS_REG(0)));

      spiHw->dma_in_link.start = 1;   // Start SPI DMA transfer (1)  M
      spiHw->dma_out_link.start = 1;   // Start SPI DMA transfer (1)  M
      spiHw->cmd.usr = 1;
      WRITE_PERI_REG(MCPWM_TIMER0_CFG1_REG(0), s1); // start timer 0
      WRITE_PERI_REG(SPI_CMD_REG(3), s0); // start SPI transfer

      WRITE_PERI_REG(MCPWM_TIMER0_SYNC_REG(0), s3);
      portENABLE_INTERRUPTS();
   }

   ESP_LOGI(TAG, "Initializing SPI.....Done");

}

ADC128S102::~ADC128S102()
{
   // Stop hardware
   WRITE_PERI_REG(MCPWM_TIMER1_CFG1_REG(0),
                  READ_PERI_REG(MCPWM_TIMER1_CFG1_REG(0)) & ~MCPWM_TIMER1_MOD_M); // stop timer 1

   aaa_spi_release_circular_buffer(VSPI_HOST, 1);
   free(out);
   free(in);
}

uint16_t ADC128S102::read_input(int index)
{
   uint8_t hb = in->buf[2 * index];
   uint8_t lb = in->buf[2 * index + 1];
   uint16_t value = (hb << 8) + lb;
   return value;
}

#undef TAG