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

#include "driver/timer.h"
#include "driver/spi_master.h"
#include "driver/mcpwm.h"
#include "driver/ledc.h"
#include <esp_log.h>
#include <esp_task_wdt.h>
#include "esp_timer.h"
#include "esp_types.h"
#include <rom/lldesc.h>
#include <soc/soc.h>
#include <soc/spi_struct.h>
#include <soc/mcpwm_reg.h>
#include <soc/mcpwm_struct.h>
#include <soc/dport_access.h>
#include <soc/dport_reg.h>
#include <soc/ledc_struct.h>
#include <soc/ledc_reg.h>
#include <sys/time.h>
#include "string.h"

#include "Demiurge.h"
#include "my_spi.h"

#define LDAC_MASK 0x10  // GPIO4
#define DEMIURGE_TIMER_GROUP TIMER_GROUP_1
#define DEMIURGE_TIMER TIMER_0

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

static portMUX_TYPE timer_mux;

void alternativeInit();

void IRAM_ATTR startTimerTask(void *parameter) {

}

void IRAM_ATTR startInfiniteTask(void *parameter) {
   ESP_LOGD(TAG, "Starting CORE 1.");
   esp_err_t error = timer_start(DEMIURGE_TIMER_GROUP, DEMIURGE_TIMER);
   ESP_ERROR_CHECK(error);
   uint64_t counter = 0;
   uint64_t next = 0;

   while (true) {
//      while (counter < next)
      {
         timer_get_counter_value(DEMIURGE_TIMER_GROUP, DEMIURGE_TIMER, &counter);
      } // WAIT
//      if (Demiurge::runtime().timing[0]->start()) {
//      Demiurge::runtime().tick();
//         Demiurge::runtime().timing[0]->stop();
//      }
      next = counter + 50000;
   }

}

#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-pro-type-member-init"

Demiurge::Demiurge() {
   ESP_LOGI(TAG, "Starting Demiurge...\n");
//   gpio_set_level(GPIO_NUM_21, 1);
//   gpio_set_level(GPIO_NUM_22, 1);
//   gpio_set_level(GPIO_NUM_25, 1);
//   gpio_set_level(GPIO_NUM_26, 1);
//   gpio_set_direction(GPIO_NUM_21, GPIO_MODE_OUTPUT);
//   gpio_set_direction(GPIO_NUM_22, GPIO_MODE_OUTPUT);
//   gpio_set_direction(GPIO_NUM_25, GPIO_MODE_OUTPUT);
//   gpio_set_direction(GPIO_NUM_26, GPIO_MODE_OUTPUT);
//
   // DAC LDAC
//   gpio_set_direction(GPIO_NUM_4, GPIO_MODE_OUTPUT);
//   gpio_set_level(GPIO_NUM_4, 0);

   alternativeInit();
//   initializeDacSpi();
//   initializeAdcSpi();
//   mcp4822_init(&_dac, _hspi);
//   adc128s102_init(&_adc, _vspi);
//   adc128s102_queue(&_adc);
//   initializeSinks();
//   pwm_high_init();
};

void Demiurge::startRuntime() {
   if (_started)
      return;
   _started = true;
   timing[0] = new Timing("tick");
   timing[1] = new Timing("processing");
   timing[2] = nullptr;
   timing[3] = nullptr;
   timing[4] = nullptr;
   initializeConcurrency();
}

void Demiurge::initializeDacSpi() {
   ESP_LOGE(TAG, "Initializing DAC.");
   int dma_chan = 1;
   if (spicommon_dma_chan_claim(dma_chan)) {
      ESP_LOGE(TAG, "Successful allocation of DMA %d", dma_chan);
      spicommon_dma_chan_free(dma_chan);
   } else {
      ESP_LOGE(TAG, "Unsuccessful allocation of DMA %d", dma_chan);
   }


   memset(&_hspiBusConfig, 0, sizeof(_hspiBusConfig));
   _hspiBusConfig.miso_io_num = -1;
   _hspiBusConfig.mosi_io_num = DEMIURGE_HMOSI_PIN;
   _hspiBusConfig.sclk_io_num = DEMIURGE_HCLK_PIN;
   _hspiBusConfig.quadwp_io_num = -1;
   _hspiBusConfig.quadhd_io_num = -1;
   _hspiBusConfig.max_transfer_sz = 6;

   memset(&_hspiDeviceIntfConfig, 0, sizeof(_hspiDeviceIntfConfig));
   _hspiDeviceIntfConfig.clock_speed_hz = 20000000;
   _hspiDeviceIntfConfig.mode = 0;                    //SPI mode 0
//   _hspiDeviceIntfConfig.spics_io_num = DEMIURGE_HCS_PIN;   //CS pin
   _hspiDeviceIntfConfig.spics_io_num = -1;   //CS pin
   _hspiDeviceIntfConfig.queue_size = 8;


   esp_err_t ret = spi_bus_initialize(HSPI_HOST, &_hspiBusConfig, dma_chan);
   ESP_ERROR_CHECK(ret);

   ret = spi_bus_add_device(HSPI_HOST, &_hspiDeviceIntfConfig, &_hspi);
   ESP_ERROR_CHECK(ret);


//   portENTER_CRITICAL(&timer_mux);
//   portEXIT_CRITICAL(&timer_mux);

   spi_dev_t *hw_spi = spicommon_hw_for_host(HSPI_HOST);
   hw_spi->dma_conf.dma_continue = true;


   ESP_LOGE(TAG, "DAC initialized.");
}

void Demiurge::initializeAdcSpi() {
   ESP_LOGE(TAG, "Initializing ADC.");
   int dma_chan = 2;
   if (spicommon_dma_chan_claim(dma_chan)) {
      ESP_LOGE(TAG, "Successful allocation of DMA %d", dma_chan);
      spicommon_dma_chan_free(dma_chan);
   } else {
      ESP_LOGE(TAG, "Unsuccessful allocation of DMA %d", dma_chan);
   }

   memset(&_vspiBusConfig, 0, sizeof(spi_bus_config_t));
   _vspiBusConfig.miso_io_num = DEMIURGE_VMISO_PIN;
   _vspiBusConfig.mosi_io_num = DEMIURGE_VMOSI_PIN;
   _vspiBusConfig.sclk_io_num = DEMIURGE_VCLK_PIN;
   _vspiBusConfig.quadwp_io_num = -1;
   _vspiBusConfig.quadhd_io_num = -1;
   _vspiBusConfig.max_transfer_sz = 32;
   _vspiBusConfig.flags = SPICOMMON_BUSFLAG_MASTER;

   memset(&_vspiDeviceIntfConfig, 0, sizeof(spi_device_interface_config_t));
   _vspiDeviceIntfConfig.clock_speed_hz = 16000000;
   _vspiDeviceIntfConfig.mode = 0;                    //SPI mode 0
   _vspiDeviceIntfConfig.spics_io_num = DEMIURGE_VCS_PIN;   //CS pin
   _vspiDeviceIntfConfig.queue_size = 8;


   esp_err_t ret = spi_bus_initialize(VSPI_HOST, &_vspiBusConfig, dma_chan);
   ESP_ERROR_CHECK(ret);


   ret = spi_bus_add_device(VSPI_HOST, &_vspiDeviceIntfConfig, &_vspi);
   ESP_ERROR_CHECK(ret);

   spi_dev_t *hw_spi = spicommon_hw_for_host(VSPI_HOST);
   hw_spi->dma_conf.dma_continue = true;

   ESP_LOGE(TAG, "ADC initialized.");
}

void Demiurge::initializeConcurrency() {

   TaskHandle_t idleTask = xTaskGetIdleTaskHandle();
   esp_task_wdt_delete(idleTask);
   xTaskCreatePinnedToCore(startInfiniteTask, "Audio", 8192, nullptr, 7, &_taskHandle, 0);
}

void Demiurge::initializeSinks() {
   for (auto &_sink : _sinks) {
      _sink = nullptr;
   }
}

#pragma clang diagnostic pop

Demiurge::~Demiurge() {

   spi_bus_remove_device(_vspi);
   spi_bus_remove_device(_hspi);

   esp_timer_stop(_timer);
   esp_timer_delete(_timer);
   free(_config);
}

void Demiurge::registerSink(signal_t *processor) {
   configASSERT(processor != nullptr)
   auto *port = (audio_out_port_t *) processor->data;
   configASSERT(port != nullptr)
   configASSERT(port->position > 0 && port->position <= DEMIURGE_MAX_SINKS)
   _sinks[port->position - 1] = processor;
}

void Demiurge::unregisterSink(signal_t *processor) {
   configASSERT(processor != nullptr)
   auto *port = (audio_out_port_t *) processor->data;
   configASSERT(port != nullptr)
   configASSERT(port->position > 0 && port->position <= DEMIURGE_MAX_SINKS)
   _sinks[port->position - 1] = nullptr;
}

void IRAM_ATTR Demiurge::tick() {
//   gpio_set_level(GPIO_NUM_21, 0);
   readGpio();
   readADC();
   timerCounter = timerCounter + 50;

   for (int i = 0; i < DEMIURGE_MAX_SINKS; i++) {
      signal_t *sink = _sinks[i];
      if (sink != nullptr) {
//         gpio_set_level(GPIO_NUM_26, 0);
         int32_t voltage = audiooutport_read(sink, timerCounter);
//         gpio_set_level(GPIO_NUM_26, 1);
         setDAC(i, voltage);
      }
   }
//   mcp4822_dacOutput(&_dac, _dac1, _dac2);
//   gpio_set_level(GPIO_NUM_21, 1);
}


void IRAM_ATTR Demiurge::readGpio() {
   _gpios = gpio_input_get(); // get all 32 gpios
}

void IRAM_ATTR Demiurge::readADC() {
//   adc128s102_read(&_adc); // get the previous cycle's data.
//   adc128s102_queue(&_adc);  // start new conversion.
//   adc128s102_sync_read(&_adc);

//   uint16_t *channels = _adc._channels;
//   int32_t k = 20 / 4095;
//   for (int i = 0; i < 8; i++) {
//      // k = (y1-y2) / (x1-x2)
//      // k = (10- -10) / (4095 - 0)
//      // k = 20 / 4095
//
//      // m = y1 - k * x1;
//      // m = 10 - k * 4095;
//      _inputs[i] = k * channels[i] - 10;
//   }
//   for( int i=0; i < 8 ; i++ ){
//      _inputs[i] = (_adc._rxdata[i] << 8) + _adc._rxdata[i+1];
//   }
}

static bool niclas1 = false;
static bool niclas2 = true;

void IRAM_ATTR Demiurge::setDAC(int channel, int32_t voltage) {
   // Convert to 12 bit DAC levels. -10V -> 0, 0 -> 2048, +10V -> 4095
   // x1 = 10
   // x2 = -10
   // y1 = 4095
   // y2 = 0
   // k = 4095 / 20 = 204.75
   // m = 4095 - k*10 = 2048
   uint16_t output = (uint16_t) (voltage * 204.75 + 2048.0);
   if (channel == 1) {
      niclas1 = !niclas1;
      uint16_t output = niclas1 ? 0x800 : 0x100;
      _dac._txdata[0] = MCP4822_CHANNEL_A | MCP4822_ACTIVE | ((output >> 8) & 0x0F);
      _dac._txdata[1] = output & 0xFF;
      _output1 = voltage;
   } else {
      niclas2 = !niclas2;
      uint16_t output = niclas2 ? 0x800 : 0x100;
      _dac2 = (uint16_t) (voltage * 204.75 + 2048.0);
      _dac._txdata[2] = MCP4822_CHANNEL_B | MCP4822_ACTIVE | ((output >> 8) & 0x0F);
      _dac._txdata[3] = output & 0xFF;
      _output2 = voltage;
   }
}

int32_t IRAM_ATTR *Demiurge::inputs() {
   return _inputs;
}

int32_t IRAM_ATTR *Demiurge::outputs() {
   return _outputs;
}

bool IRAM_ATTR Demiurge::gpio(int pin) {
   return (_gpios & (1 << pin)) != 0;
}

void Demiurge::printReport() {
   for (auto &t : timing) {
      if (t != nullptr)
         t->report();
   }
}

int32_t Demiurge::output1() {
   return _output1;
}

int32_t Demiurge::output2() {
   return _output2;
}

uint16_t Demiurge::dac1() {
   return _dac1;
}

uint16_t Demiurge::dac2() {
   return _dac2;
}

uint16_t *Demiurge::rawAdc() {
   return _adc._channels;
}


static lldesc_t descs{};

void led_timers(void *dummy) {
   ledc_timer_config_t ledc_timer;

   ledc_timer.speed_mode = LEDC_HIGH_SPEED_MODE;  // timer mode
   ledc_timer.duty_resolution = LEDC_TIMER_6_BIT; // resolution of PWM duty
   ledc_timer.timer_num = LEDC_TIMER_0;           // timer index
   ledc_timer.freq_hz = 250000;                    // frequency of PWM signal
//         .clk_cfg = LEDC_AUTO_CLK,            // Auto select the source clock
   ledc_timer_config(&ledc_timer);

   ledc_channel_config_t ledc_channel{
         .gpio_num   = DEMIURGE_HCS_PIN,
         .speed_mode = LEDC_HIGH_SPEED_MODE,
         .channel    = LEDC_CHANNEL_0,
         .intr_type  = LEDC_INTR_DISABLE,
         .timer_sel  = LEDC_TIMER_0,
         .duty       = 8,
         .hpoint     = 1
   };

   ledc_channel_config(&ledc_channel);
}

void alternativeInit() {

   ESP_LOGE(TAG, "Initializing SPI.\n");
   memset((void *) &descs, 0, sizeof(lldesc_t));
   descs.size = 5;
   descs.length = 5;
   descs.offset = 0;
   descs.sosf = 0;
   descs.eof = 0;
   descs.owner = 1;
   descs.qe.stqe_next = &descs;
   descs.buf = static_cast<uint8_t *>(heap_caps_malloc(16, MALLOC_CAP_DMA));
   descs.buf[0] = 0xFF;
   descs.buf[1] = 0x00;
   descs.buf[2] = 0x00;
   descs.buf[3] = 0xFF;
   descs.buf[4] = 0x00;
   descs.buf[5] = 0x00;
   esp_err_t error = myspi_prepare_circular_buffer(HSPI_HOST, 1, &descs, 10000000, GPIO_NUM_13, 1);
   ESP_ERROR_CHECK(error)
   led_timers(nullptr);
   ESP_LOGE(TAG, "Initializing SPI.....Done\n");
}