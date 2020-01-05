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
#include <driver/ledc.h>
#include <esp_log.h>
#include <soc/mcpwm_struct.h>
#include <soc/mcpwm_reg.h>
#include <driver/mcpwm.h>
#include <driver/periph_ctrl.h>
#include "driver/spi_master.h"

#include "MCP4822.h"
#include "../Demiurge.h"
#include "../aaa_spi.h"

static void initialize(gpio_num_t pin_out) {
   ESP_LOGE(TAG, "Initializing DAC timer.");

   // We use MCPWM0 TIMER 0 for CS generation to DAC.

   PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[pin_out], PIN_FUNC_GPIO);
   gpio_set_direction(pin_out, GPIO_MODE_INPUT_OUTPUT);
   gpio_matrix_out(pin_out, PWM0_OUT0A_IDX, false, false);
   periph_module_enable(PERIPH_PWM0_MODULE);

   WRITE_PERI_REG(MCPWM_TIMER_SYNCI_CFG_REG(0) , (1 << MCPWM_TIMER0_SYNCISEL_S ));
   WRITE_PERI_REG(MCPWM_OPERATOR_TIMERSEL_REG(0), (0 << MCPWM_OPERATOR0_TIMERSEL));

   WRITE_PERI_REG(MCPWM_GEN0_TSTMP_A_REG(0), 32);  // 32 cycles LOW after UTEZ.
   WRITE_PERI_REG(MCPWM_GEN0_A_REG(0) , (1 << MCPWM_GEN0_A_UTEZ_S ) | (2 << MCPWM_GEN0_A_UTEA_S )); // UTEZ= set PWM0A low, UTEA=set PWM0A high

   WRITE_PERI_REG(MCPWM_TIMER0_CFG0_REG(0) , 15 << MCPWM_TIMER0_PRESCALE_S | 39 << MCPWM_TIMER0_PERIOD_S );    // Prescale=16, so timer is 10MHz, 40 clocks per cycle
   WRITE_PERI_REG(MCPWM_TIMER0_CFG1_REG(0) , (1 << MCPWM_TIMER0_MOD_S) | (2 << MCPWM_TIMER0_START_S) );        // Continuously running, decrease mode.


   ESP_LOGE(TAG, "Initializing DAC timer....Done");
}

MCP4822::MCP4822(gpio_num_t mosi_pin, gpio_num_t sclk_pin, gpio_num_t cs_pin) {
   ESP_LOGE(TAG, "Initializing SPI.");
   initialize(cs_pin);

   descs = static_cast<lldesc_t *>(heap_caps_malloc(sizeof(lldesc_t), MALLOC_CAP_DMA));
   memset((void *) descs, 0, sizeof(lldesc_t));
   descs->size = 5;
   descs->length = 5;
   descs->offset = 0;
   descs->sosf = 0;
   descs->eof = 0;
   descs->owner = 1;
   descs->qe.stqe_next = descs;
   descs->buf = static_cast<uint8_t *>(heap_caps_malloc(16, MALLOC_CAP_DMA));
   descs->buf[0] = 0x00;
   descs->buf[1] = 0x00;
   descs->buf[2] = 0x00;
   descs->buf[3] = 0x00;
   descs->buf[4] = 0x55;
   esp_err_t error = aaa_spi_prepare_circular_buffer(HSPI_HOST, 1, descs, 10000000, mosi_pin, sclk_pin, 13);

   // this bit of code makes sure both timers and SPI transfer are started as close together as possible
   portDISABLE_INTERRUPTS();
   auto s0 = 1 << SPI_USR_S;
   auto s1 = (1 << MCPWM_TIMER0_MOD_S) | (2 << MCPWM_TIMER0_START_S);
   auto s3 = (16 << MCPWM_TIMER0_PHASE_S) | (0 << MCPWM_TIMER1_SYNCO_SEL) | (1 << MCPWM_TIMER1_SYNC_SW_S);

   WRITE_PERI_REG( MCPWM_TIMER0_CFG1_REG(0), s1 ); // start timer 0
   WRITE_PERI_REG(SPI_CMD_REG(3), s0); // start SPI transfer

   WRITE_PERI_REG(MCPWM_TIMER0_SYNC_REG(0), s3);
   portENABLE_INTERRUPTS();

   ESP_ERROR_CHECK(error)
   ESP_LOGE(TAG, "Initializing SPI.....Done");
}

void MCP4822::setOutput(int channel, uint16_t output) {
   if (channel == 1) {
      descs->buf[0] = MCP4822_CHANNEL_A | MCP4822_ACTIVE | ((output >> 8) & 0x0F);
      descs->buf[1] = output & 0xFF;
   } else {
      descs->buf[2] = MCP4822_CHANNEL_B | MCP4822_ACTIVE | ((output >> 8) & 0x0F);
      descs->buf[3] = output & 0xFF;
   }
}

MCP4822::~MCP4822() {
   ESP_LOGE(TAG, "Destruction of MCP4822");

   // Stop hardware
   WRITE_PERI_REG(SPI_CMD_REG(3), READ_PERI_REG(SPI_CMD_REG(3)) & ~SPI_USR_M); // stop SPI transfer
   WRITE_PERI_REG(MCPWM_TIMER0_CFG1_REG(0), READ_PERI_REG(MCPWM_TIMER0_CFG1_REG(0)) & ~MCPWM_TIMER0_MOD_M); // stop timer 0
   WRITE_PERI_REG(MCPWM_TIMER1_CFG1_REG(0), READ_PERI_REG(MCPWM_TIMER1_CFG1_REG(0)) & ~MCPWM_TIMER1_MOD_M); // stop timer 1

   aaa_spi_release_circular_buffer(HSPI_HOST, 1, GPIO_NUM_13, GPIO_NUM_14);
   free( descs );
}
