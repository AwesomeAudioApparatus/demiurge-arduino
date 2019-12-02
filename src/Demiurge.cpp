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
#include "esp_types.h"
#include "esp_timer.h"
#include "string.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/periph_ctrl.h"
#include "driver/timer.h"
#include "driver/spi_master.h"
#include "Demiurge.h"

#define LDAC_MASK 0x10  // GPIO4

portMUX_TYPE demiurgeTimerMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR onTimer(void *parameter) {
   if (Demiurge::runtime().timing[0]->start()) {
      portENTER_CRITICAL_ISR(&demiurgeTimerMux);
      Demiurge::runtime().tick();
      portEXIT_CRITICAL_ISR(&demiurgeTimerMux);
      Demiurge::runtime().timing[0]->stop();
   }
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-pro-type-member-init"

Demiurge::Demiurge() {
   begin();
};

void Demiurge::begin() {
   if (_started)
      return;
   _started = true;
   timing[0] = new Timing("onTimer");
   timing[1] = new Timing("ADC");
   timing[2] = new Timing("DAC");
   timing[3] = nullptr;
   timing[4] = nullptr;
   initializeDacSpi();
   initializeAdcSpi();
   _dac = new MCP4822(_hspi);
   _adc = new ADC128S102(_hspi);
   _adc->queue();
   initializeTimer();
   initializeSinks();
}

void Demiurge::initializeDacSpi() {
   memset(&_hspiBusConfig, 0, sizeof(_hspiBusConfig));
   _hspiBusConfig.miso_io_num = -1;
   _hspiBusConfig.mosi_io_num = DEMIURGE_HMOSI_PIN;
   _hspiBusConfig.sclk_io_num = DEMIURGE_HCLK_PIN;
   _hspiBusConfig.quadwp_io_num = -1;
   _hspiBusConfig.quadhd_io_num = -1;
   _hspiBusConfig.max_transfer_sz = 16;
   //_hspiBusConfig.flags = SPICOMMON_BUSFLAG_MASTER;
   //_hspiBusConfig.intr_flags = ESP_INTR_FLAG_IRAM;

   memset(&_hspiDeviceIntfConfig, 0, sizeof(_hspiDeviceIntfConfig));
   _hspiDeviceIntfConfig.clock_speed_hz = 10000000;
   _hspiDeviceIntfConfig.mode = 0;                    //SPI mode 0
   _hspiDeviceIntfConfig.spics_io_num = DEMIURGE_HCS_PIN;   //CS pin
   _hspiDeviceIntfConfig.queue_size = 8;
//   _hspiDeviceIntfConfig.flags = SPI_DEVICE_HALFDUPLEX | SPI_DEVICE_NO_DUMMY;
//   _hspiDeviceIntfConfig.command_bits = 0;
//   _hspiDeviceIntfConfig.address_bits = 0;

   esp_err_t ret = spi_bus_initialize(HSPI_HOST, &_hspiBusConfig, 1);
   ESP_ERROR_CHECK(ret);

   ret = spi_bus_add_device(HSPI_HOST, &_hspiDeviceIntfConfig, &_hspi);
   ESP_ERROR_CHECK(ret);
}

void Demiurge::initializeAdcSpi() {
   memset(&_vspiBusConfig, 0, sizeof(_vspiBusConfig));
   _vspiBusConfig.miso_io_num = DEMIURGE_VMISO_PIN;
   _vspiBusConfig.mosi_io_num = DEMIURGE_VMOSI_PIN;
   _vspiBusConfig.sclk_io_num = DEMIURGE_VCLK_PIN;
   _vspiBusConfig.quadwp_io_num = -1;
   _vspiBusConfig.quadhd_io_num = -1;
   _vspiBusConfig.max_transfer_sz = 32;
//   _vspiBusConfig.flags = SPICOMMON_BUSFLAG_MASTER;
//   _vspiBusConfig.intr_flags = ESP_INTR_FLAG_IRAM;

   memset(&_vspiDeviceIntfConfig, 0, sizeof(_vspiDeviceIntfConfig));
   _vspiDeviceIntfConfig.clock_speed_hz = 10000000;
   _vspiDeviceIntfConfig.mode = 0;                    //SPI mode 0
   _vspiDeviceIntfConfig.spics_io_num = DEMIURGE_VCS_PIN;   //CS pin
   _vspiDeviceIntfConfig.queue_size = 8;
//   _vspiDeviceIntfConfig.flags = SPI_DEVICE_HALFDUPLEX | SPI_DEVICE_NO_DUMMY;
//   _vspiDeviceIntfConfig.command_bits = 0;
//   _vspiDeviceIntfConfig.address_bits = 0;

   esp_err_t ret = spi_bus_initialize(VSPI_HOST, &_vspiBusConfig, 2);
   ESP_ERROR_CHECK(ret);

   ret = spi_bus_add_device(HSPI_HOST, &_vspiDeviceIntfConfig, &_vspi);
   ESP_ERROR_CHECK(ret);
}

void Demiurge::initializeTimer() {
   _config = static_cast<esp_timer_create_args_t *>(malloc(sizeof(esp_timer_create_args_t)));
   _config->callback = onTimer;
   _config->name = "Sampler";
   _config->dispatch_method = ESP_TIMER_TASK;

   esp_err_t timerError = esp_timer_create(_config, &_timer);
   ESP_ERROR_CHECK(timerError)

   // TODO: We can not run faster than 50 microseconds on soft-ish timers. Use hardware timers??
   timerError = esp_timer_start_periodic(_timer, 500);
   ESP_ERROR_CHECK(timerError)
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

void Demiurge::registerSink(Sink *processor) {
   for (auto &_sink : _sinks) {
      if (_sink == nullptr) {
         _sink = processor;
         break;
      }
   }
}

void Demiurge::unregisterSink(Sink *processor) {
   for (auto &_sink : _sinks) {
      if (_sink == processor) {
         _sink = nullptr;
         break;
      }
   }
}

void IRAM_ATTR Demiurge::tick() {
   readGpio();
   readADC();
   timerCounter = timerCounter + 50;
   for (auto &_sink : _sinks) {
      if (_sink != nullptr) {
         _sink->tick(timerCounter);
      }
   }
}


void Demiurge::readGpio() {
   _gpios = gpio_input_get(); // get all 32 gpios
}

void IRAM_ATTR Demiurge::readADC() {
   timing[1]->start();

   _adc->read(); // get the previous cycle's data.
   _adc->queue();  // start new conversion.
   uint16_t *channels = _adc->channels();
   double k = 20 / 4095;
   for (int i = 0; i < 8; i++) {
      // k = (y1-y2) / (x1-x2)
      // k = (10- -10) / (4095 - 0)
      // k = 20 / 4095

      // m = y1 - k * x1;
      // m = 10 - k * 4095;
      _inputs[i] = k * channels[i] - 10;
   }
   timing[1]->stop();
}

void Demiurge::setDAC(int channel, double voltage) {
   timing[2]->start();
   // Convert to 12 bit DAC levels. -10V -> 0, 0 -> 2048, +10V -> 4095
   auto output = (uint16_t) (voltage * 204.75 + 2048.0);

   // x1 = 10
   // x2 = -10
   // y1 = 4095
   // y2 = 0
   // k = 4095 / 20 = 204.75
   // m = 4095 - k*10 = 2048
   esp_err_t dacError = ESP_OK;
   gpio_output_set(LDAC_MASK, 0, LDAC_MASK, 0);
   if (channel == 1) {
      dacError = _dac->send(MCP4822_CHANNEL_A | MCP4822_GAIN | MCP4822_ACTIVE, output);
      _dac1 = output;
      _output1 = voltage;
   }
   if (channel == 2) {
      dacError = _dac->send(MCP4822_CHANNEL_B | MCP4822_GAIN | MCP4822_ACTIVE, output);
      _dac2 = output;
      _output2 = voltage;
   }
   gpio_output_set(0, LDAC_MASK, LDAC_MASK, 0);
   ESP_ERROR_CHECK(dacError)

   _outputs[channel - 1] = voltage;
   timing[2]->stop();
}

double *Demiurge::inputs() {
   return _inputs;
}

double *Demiurge::outputs() {
   return _outputs;
}

bool Demiurge::gpio(int pin) {
   return (_gpios & (1 << pin)) != 0;
}

void Demiurge::printReport() {
   for (auto &t : timing) {
      if (t != nullptr)
         t->report();
   }
}
double Demiurge::output1() {
   return _output1;
}
double Demiurge::output2() {
   return _output2;
}

uint16_t Demiurge::dac1() {
   return _dac1;
}

uint16_t Demiurge::dac2() {
   return _dac2;
}


