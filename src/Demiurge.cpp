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

#include <sys/time.h>
#include <esp_task_wdt.h>
#include "esp_types.h"
#include "esp_timer.h"
#include "string.h"
#include "driver/timer.h"
#include "driver/spi_master.h"
#include "Demiurge.h"

#define LDAC_MASK 0x10  // GPIO4
#define DEMIURGE_TIMER_GROUP TIMER_GROUP_1
#define DEMIURGE_TIMER TIMER_0

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

void IRAM_ATTR startTimerTask(void *parameter) {

}

void IRAM_ATTR startInfiniteTask(void *parameter) {
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
         Demiurge::runtime().tick();
//         Demiurge::runtime().timing[0]->stop();
//      }
      next = counter + 50;
   }

}

#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-pro-type-member-init"

Demiurge::Demiurge() {
   gpio_set_level(GPIO_NUM_21, 1);
   gpio_set_level(GPIO_NUM_22, 1);
   gpio_set_level(GPIO_NUM_25, 1);
   gpio_set_level(GPIO_NUM_26, 1);
   gpio_set_direction(GPIO_NUM_21, GPIO_MODE_OUTPUT);
   gpio_set_direction(GPIO_NUM_22, GPIO_MODE_OUTPUT);
   gpio_set_direction(GPIO_NUM_25, GPIO_MODE_OUTPUT);
   gpio_set_direction(GPIO_NUM_26, GPIO_MODE_OUTPUT);

   initializeDacSpi();
   initializeAdcSpi();
   mcp4822_init(&_dac, _hspi);
   adc128s102_init(&_adc, _vspi);
   adc128s102_queue(&_adc);
   initializeSinks();
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
   memset(&_hspiBusConfig, 0, sizeof(_hspiBusConfig));
   _hspiBusConfig.miso_io_num = -1;
   _hspiBusConfig.mosi_io_num = DEMIURGE_HMOSI_PIN;
   _hspiBusConfig.sclk_io_num = DEMIURGE_HCLK_PIN;
   _hspiBusConfig.quadwp_io_num = -1;
   _hspiBusConfig.quadhd_io_num = -1;
   _hspiBusConfig.max_transfer_sz = 16;

   memset(&_hspiDeviceIntfConfig, 0, sizeof(_hspiDeviceIntfConfig));
   _hspiDeviceIntfConfig.clock_speed_hz = 10000000;
   _hspiDeviceIntfConfig.mode = 0;                    //SPI mode 0
   _hspiDeviceIntfConfig.spics_io_num = DEMIURGE_HCS_PIN;   //CS pin
   _hspiDeviceIntfConfig.queue_size = 8;

   esp_err_t ret = spi_bus_initialize(HSPI_HOST, &_hspiBusConfig, 1);
   ESP_ERROR_CHECK(ret);

   ret = spi_bus_add_device(HSPI_HOST, &_hspiDeviceIntfConfig, &_hspi);
   ESP_ERROR_CHECK(ret);
}

void Demiurge::initializeAdcSpi() {
   memset(&_vspiBusConfig, 0, sizeof(spi_bus_config_t));
   _vspiBusConfig.miso_io_num = DEMIURGE_VMISO_PIN;
   _vspiBusConfig.mosi_io_num = DEMIURGE_VMOSI_PIN;
   _vspiBusConfig.sclk_io_num = DEMIURGE_VCLK_PIN;
   _vspiBusConfig.quadwp_io_num = -1;
   _vspiBusConfig.quadhd_io_num = -1;
   _vspiBusConfig.max_transfer_sz = 32;
   _vspiBusConfig.flags = SPICOMMON_BUSFLAG_MASTER;

   memset(&_vspiDeviceIntfConfig, 0, sizeof(spi_device_interface_config_t));
   _vspiDeviceIntfConfig.clock_speed_hz = 10000000;
   _vspiDeviceIntfConfig.mode = 0;                    //SPI mode 0
   _vspiDeviceIntfConfig.spics_io_num = DEMIURGE_VCS_PIN;   //CS pin
   _vspiDeviceIntfConfig.queue_size = 8;

   esp_err_t ret = spi_bus_initialize(VSPI_HOST, &_vspiBusConfig, 2);
   ESP_ERROR_CHECK(ret);

   ret = spi_bus_add_device(VSPI_HOST, &_vspiDeviceIntfConfig, &_vspi);
   ESP_ERROR_CHECK(ret);

   spi_device_acquire_bus(_vspi, portMAX_DELAY);
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

uint16_t testing_call(int i) {
   float x = (float) i;
   return (uint16_t) (x * 204.75 + 2048.0);
}

void IRAM_ATTR Demiurge::tick() {
//   readGpio();
//   readADC();
   timerCounter = timerCounter + 50;

   gpio_set_level(GPIO_NUM_26, 0);
   gpio_set_level(GPIO_NUM_26, 1);
   for (int i = 0; i < DEMIURGE_MAX_SINKS; i++) {
      signal_t *sink = _sinks[i];
      if (sink != nullptr) {
         gpio_set_level(GPIO_NUM_25, 0);
         gpio_set_level(GPIO_NUM_25, 1);

         gpio_set_level(GPIO_NUM_25, 0);
         gpio_set_level(GPIO_NUM_25, 1);
         gpio_set_level(GPIO_NUM_25, 0);
         gpio_set_level(GPIO_NUM_25, 1);
         float voltage = audiooutport_read(sink, timerCounter);
         gpio_set_level(GPIO_NUM_25, 0);
         gpio_set_level(GPIO_NUM_25, 1);
         gpio_set_level(GPIO_NUM_25, 0);
         gpio_set_level(GPIO_NUM_25, 1);
         setDAC(i, voltage);
         gpio_set_level(GPIO_NUM_25, 0);
         gpio_set_level(GPIO_NUM_25, 1);
         gpio_set_level(GPIO_NUM_25, 0);
         gpio_set_level(GPIO_NUM_25, 1);
      }
   }
//   gpio_output_set(LDAC_MASK, 0, LDAC_MASK, 1);
   mcp4822_dacOutput(&_dac, _dac1, _dac2);
   gpio_output_set(0, LDAC_MASK, LDAC_MASK, 0);
}


void IRAM_ATTR Demiurge::readGpio() {
   _gpios = gpio_input_get(); // get all 32 gpios
}

void IRAM_ATTR Demiurge::readADC() {
   adc128s102_read(&_adc); // get the previous cycle's data.
   adc128s102_queue(&_adc);  // start new conversion.
   uint16_t *channels = _adc._channels;
   float k = 20 / 4095;
   for (int i = 0; i < 8; i++) {
      // k = (y1-y2) / (x1-x2)
      // k = (10- -10) / (4095 - 0)
      // k = 20 / 4095

      // m = y1 - k * x1;
      // m = 10 - k * 4095;
      _inputs[i] = k * channels[i] - 10;
   }
}

void IRAM_ATTR Demiurge::setDAC(int channel, float voltage) {
   // Convert to 12 bit DAC levels. -10V -> 0, 0 -> 2048, +10V -> 4095
   // x1 = 10
   // x2 = -10
   // y1 = 4095
   // y2 = 0
   // k = 4095 / 20 = 204.75
   // m = 4095 - k*10 = 2048
   if (channel == 1) {
      _dac1 = (uint16_t) (voltage * 204.75 + 2048.0);
      _output1 = voltage;
   } else {
      _dac2 = (uint16_t) (voltage * 204.75 + 2048.0);
      _output2 = voltage;
   }
}

float IRAM_ATTR *Demiurge::inputs() {
   return _inputs;
}

float IRAM_ATTR *Demiurge::outputs() {
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

float Demiurge::output1() {
   return _output1;
}

float Demiurge::output2() {
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

