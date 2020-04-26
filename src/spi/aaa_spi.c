
#include <soc/spi_reg.h>
#include <soc/dport_reg.h>
#include <soc/spi_struct.h>
#include <driver/spi_common.h>
#include <driver/gpio.h>
#include "aaa_spi.h"

static const uint8_t InvalidIndex = (uint8_t) -1;

spi_dev_t *aaa_spi_get_hw_for_host(
      spi_host_device_t host
) {
   switch (host) {
      case SPI_HOST:
         return &SPI1;
      case HSPI_HOST:
         return &SPI2;
      case VSPI_HOST:
         return &SPI3;
      default:
         return NULL;
   }
}


static uint8_t getSpidOutByHost(
      spi_host_device_t host
) {
   switch (host) {
      case SPI_HOST:
         return SPID_OUT_IDX;
      case HSPI_HOST:
         return HSPID_OUT_IDX;
      case VSPI_HOST:
         return VSPID_OUT_IDX;
      default:
         return InvalidIndex;
   }
}


static uint8_t getSpidInByHost(spi_host_device_t host) {
   switch (host) {
      case SPI_HOST:
         return SPID_IN_IDX;
      case HSPI_HOST:
         return HSPID_IN_IDX;
      case VSPI_HOST:
         return VSPID_IN_IDX;
      default:
         return InvalidIndex;
   }
}

static uint8_t getSpiClkOutByHost(spi_host_device_t host) {
   switch (host) {
      case SPI_HOST:
         return SPICLK_OUT_IDX;
      case HSPI_HOST:
         return HSPICLK_OUT_IDX;
      case VSPI_HOST:
         return VSPICLK_OUT_IDX;
      default:
         return InvalidIndex;
   }
}

static uint8_t getSpiClkInByHost(spi_host_device_t host) {
   switch (host) {
      case SPI_HOST:
         return SPICLK_IN_IDX;
      case HSPI_HOST:
         return HSPICLK_IN_IDX;
      case VSPI_HOST:
         return VSPICLK_IN_IDX;
      default:
         return InvalidIndex;
   }
}


esp_err_t aaa_spi_prepare_circular(const spi_host_device_t spiHostDevice, const int dma_chan,
                                   const lldesc_t *lldescs_out, const lldesc_t *lldescs_in,
                                   const long dmaClockSpeedInHz, const gpio_num_t mosi_gpio_num,
                                   const gpio_num_t miso_gpio_num, const gpio_num_t sclk_gpio_num,
                                   const int waitCycle) {

   const bool spi_periph_claimed = spicommon_periph_claim(spiHostDevice);
   if (!spi_periph_claimed) {
      return DEMIURGE_ESP_ERR_SPI_HOST_ALREADY_IN_USE;
   }

   const bool dma_chan_claimed = spicommon_dma_chan_claim(dma_chan);
   if (!dma_chan_claimed) {
      spicommon_periph_free(spiHostDevice);
      return DEMIURGE_ESP_ERR_SPI_DMA_ALREADY_IN_USE;
   }

   spi_dev_t *const spiHw = aaa_spi_get_hw_for_host(spiHostDevice);

   //Use GPIO
   PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[mosi_gpio_num], PIN_FUNC_GPIO);
   PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[sclk_gpio_num], PIN_FUNC_GPIO);
   PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[miso_gpio_num], PIN_FUNC_GPIO);

   gpio_set_direction(mosi_gpio_num, GPIO_MODE_INPUT_OUTPUT);
   gpio_set_direction(sclk_gpio_num, GPIO_MODE_INPUT_OUTPUT);
   if (miso_gpio_num < GPIO_NUM_MAX) {
      gpio_set_direction(miso_gpio_num, GPIO_MODE_INPUT);
      gpio_set_pull_mode(miso_gpio_num, GPIO_PULLUP_ONLY);
   }

   gpio_matrix_out(mosi_gpio_num, getSpidOutByHost(spiHostDevice), false, false);
   gpio_matrix_in(mosi_gpio_num, getSpidInByHost(spiHostDevice), false);

   gpio_matrix_out(sclk_gpio_num, getSpiClkOutByHost(spiHostDevice), false, false);
   gpio_matrix_in(sclk_gpio_num, getSpiClkInByHost(spiHostDevice), false);

   //Select DMA channel.
   DPORT_SET_PERI_REG_BITS(DPORT_SPI_DMA_CHAN_SEL_REG, 3, dma_chan, (spiHostDevice * 2));

   //Reset DMA
   spiHw->dma_conf.val |= SPI_OUT_RST | SPI_IN_RST | SPI_AHBM_RST | SPI_AHBM_FIFO_RST;
   spiHw->dma_out_link.start = 0;
   spiHw->dma_in_link.start = 0;
   spiHw->dma_conf.val &= ~(SPI_OUT_RST | SPI_IN_RST | SPI_AHBM_RST | SPI_AHBM_FIFO_RST);

   spiHw->ctrl.val = 0;
   spiHw->user.val = 0;

   //Reset timing
   spiHw->ctrl2.val = 0;

   //Disable unneeded ints
   spiHw->slave.rd_buf_done = 0;
   spiHw->slave.wr_buf_done = 0;
   spiHw->slave.rd_sta_done = 0;
   spiHw->slave.wr_sta_done = 0;
   spiHw->slave.rd_buf_inten = 0;
   spiHw->slave.wr_buf_inten = 0;
   spiHw->slave.rd_sta_inten = 0;
   spiHw->slave.wr_sta_inten = 0;
   spiHw->slave.trans_inten = 0;
   spiHw->slave.trans_done = 0;

   // Set SPI Clock
   //  Register 7.7: SPI_CLOCK_REG (0x18)
   //
   //		SPI_CLK_EQU_SYSCLK
   //			In master mode, when this bit is set to 1, spi_clk is equal
   //			to system clock; when set to 0, spi_clk is divided from system
   //			clock.
   //
   //		SPI_CLKDIV_PRE
   //			In master mode, the value of this register field is the
   //			pre-divider value for spi_clk, minus one.
   //
   //		SPI_CLKCNT_N
   //			In master mode, this is the divider for spi_clk minus one.
   //			The spi_clk frequency is
   //				system_clock/(SPI_CLKDIV_PRE+1)/(SPI_CLKCNT_N+1).
   //
   //		SPI_CLKCNT_H
   //			For a 50% duty cycle, set this to floor((SPI_CLKCNT_N+1)/2-1)
   //
   //		SPI_CLKCNT_L
   //			In master mode, this must be equal to SPI_CLKCNT_N.
   {
      const double preDivider = 1.0;
      const double apbClockSpeedInHz = APB_CLK_FREQ;
      const double apbClockPerDmaCycle = (apbClockSpeedInHz / preDivider / dmaClockSpeedInHz);

      const int32_t clkdiv_pre = ((int32_t) preDivider) - 1;
      const int32_t clkcnt_n = ((int32_t) apbClockPerDmaCycle) - 1;
      const int32_t clkcnt_h = (clkcnt_n + 1) / 2 - 1;
      const int32_t clkcnt_l = clkcnt_n;

      spiHw->clock.clk_equ_sysclk = 0;
      spiHw->clock.clkcnt_n = clkcnt_n;
      spiHw->clock.clkdiv_pre = clkdiv_pre;
      spiHw->clock.clkcnt_h = clkcnt_h;
      spiHw->clock.clkcnt_l = clkcnt_l;
   }

   //Configure bit order
   spiHw->ctrl.rd_bit_order = 0;    // MSB first
   spiHw->ctrl.wr_bit_order = 0;    // MSB first

   spiHw->pin.ck_dis = 0;

   //Configure polarity
   spiHw->pin.ck_idle_edge = 0;
   spiHw->user.ck_out_edge = 0;
   spiHw->ctrl2.miso_delay_mode = 0;

   //configure dummy bits
   spiHw->user.usr_dummy = 0;
   spiHw->user1.usr_dummy_cyclelen = 0;

   //Configure misc stuff
   spiHw->user.sio = 0;

   spiHw->ctrl2.setup_time = 0;
   spiHw->user.cs_setup = 0;
   spiHw->ctrl2.hold_time = 0;
   spiHw->user.cs_hold = 0;

   //Configure CS pin
   spiHw->pin.cs0_dis = 1;
   spiHw->pin.cs1_dis = 1;
   spiHw->pin.cs2_dis = 1;

   //Reset SPI peripheral
   spiHw->dma_conf.val |= SPI_OUT_RST | SPI_IN_RST | SPI_AHBM_RST | SPI_AHBM_FIFO_RST;
   spiHw->dma_out_link.start = 0;
   spiHw->dma_in_link.start = 0;
   spiHw->dma_conf.val &= ~(SPI_OUT_RST | SPI_IN_RST | SPI_AHBM_RST | SPI_AHBM_FIFO_RST);
   spiHw->dma_conf.out_data_burst_en = 1;

   //DMA temporary workaround: let RX DMA work somehow to avoid the issue in ESP32 v0/v1 silicon
//   spiHw->dma_in_link.addr = 0;
//   spiHw->dma_in_link.start = 1;

//   spiHw->user1.usr_addr_bitlen = 0;
//   spiHw->user2.usr_command_bitlen = 0;
   spiHw->user.usr_addr = 0;
   spiHw->user.usr_command = 0;

   if (waitCycle <= 0) {
      spiHw->user.usr_dummy = 0;
      spiHw->user1.usr_dummy_cyclelen = 0;
   } else {
      spiHw->user.usr_dummy = 1;
      spiHw->user1.usr_dummy_cyclelen = (uint8_t) (waitCycle - 1);
   }

   spiHw->user.sio = 0;
   spiHw->user.usr_mosi_highpart = 0;
   spiHw->user2.usr_command_value = 0;
   spiHw->addr = 0;
   spiHw->dma_out_link.addr = (uint64_t) (lldescs_out) & 0xFFFFF;
   if (miso_gpio_num == GPIO_NUM_MAX) {
      // Half-duplex mode with only MOSI.
      spiHw->user.doutdin = 0;
      spiHw->user.usr_mosi = 1;        // Enable MOSI
      spiHw->user.usr_miso = 0;
   } else {
      spiHw->user.usr_mosi = 1;
      spiHw->user.usr_miso = 1;
      spiHw->user.doutdin = 1;
      spiHw->dma_in_link.addr = (uint64_t) (lldescs_in) & 0xFFFFF;
   }

   spiHw->mosi_dlen.usr_mosi_dbitlen = 0;      // works great! (there's no glitch in 5 hours)
   spiHw->miso_dlen.usr_miso_dbitlen = 160;

   // Set circular mode
   //      https://www.esp32.com/viewtopic.php?f=2&t=4011#p18107
   //      > yes, in SPI DMA mode, SPI will alway transmit and receive
   //      > data when you set the SPI_DMA_CONTINUE(BIT16) of SPI_DMA_CONF_REG.

   spiHw->dma_conf.dma_tx_stop = 1;    // Stop SPI DMA
   spiHw->dma_conf.dma_rx_stop = 1;    // Stop SPI DMA
   spiHw->ctrl2.val = 0;               // Reset timing
   spiHw->dma_conf.dma_tx_stop = 0;    // Disable stop
   spiHw->dma_conf.dma_rx_stop = 0;    // Disable stop
   spiHw->dma_conf.dma_continue = 1;   // Set contiguous mode

   // These two are moved into the critical timing section in MCP4822 and ADS128S102
//   spiHw->dma_out_link.start = 1;   // Start SPI DMA transfer (1)  M
//   spiHw->cmd.usr = 1;   // SPI: Start SPI DMA transfer
   return ESP_OK;
}


esp_err_t aaa_spi_release_circular_buffer(spi_host_device_t host, int dma_chan) {

   spi_dev_t *const spiHw = aaa_spi_get_hw_for_host(host);

   spiHw->dma_conf.dma_continue = 0;
   spiHw->dma_out_link.start = 0;
   spiHw->cmd.usr = 0;

   // TODO : Reset GPIO Matrix

   spicommon_dma_chan_free(dma_chan);
   spicommon_periph_free(host);
   return ESP_OK;
}