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
#include "driver/mcpwm.h"
#include "driver/ledc.h"
#include <esp_log.h>
#include <esp_task_wdt.h>
#include "esp_timer.h"
#include "esp_types.h"
#include <rom/lldesc.h>
#include <soc/soc.h>
#include <soc/mcpwm_reg.h>
#include <soc/mcpwm_struct.h>
#include <soc/dport_access.h>
#include <soc/dport_reg.h>
#include <soc/ledc_struct.h>
#include <soc/ledc_reg.h>
#include <soc/timer_group_reg.h>
#include <sys/time.h>
#include "string.h"

#include "Demiurge.h"
#include "aaa_spi.h"

#define DEMIURGE_TIMER_GROUP 0

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

static void IRAM_ATTR initialize_tick_timer() {
   WRITE_PERI_REG(TIMG_T0CONFIG_REG(DEMIURGE_TIMER_GROUP),
                  (1 << TIMG_T0_DIVIDER_S ) | TIMG_T0_EN | TIMG_T0_AUTORELOAD | TIMG_T0_INCREASE | TIMG_T0_ALARM_EN);

   WRITE_PERI_REG(TIMG_T0LOADHI_REG(DEMIURGE_TIMER_GROUP), 0);
   WRITE_PERI_REG(TIMG_T0LOADLO_REG(DEMIURGE_TIMER_GROUP), 0);
   WRITE_PERI_REG(TIMG_T0ALARMHI_REG(DEMIURGE_TIMER_GROUP), 0);
   WRITE_PERI_REG(TIMG_T0ALARMLO_REG(DEMIURGE_TIMER_GROUP), 400);

   WRITE_PERI_REG(TIMG_T0LOAD_REG(DEMIURGE_TIMER_GROUP), 1);
}

static bool toggle = false;

static void IRAM_ATTR wait_timer_alarm() {
   gpio_set_level(GPIO_NUM_21, toggle);
   toggle = !toggle;
   while (READ_PERI_REG(TIMG_T0CONFIG_REG(DEMIURGE_TIMER_GROUP)) & TIMG_T0_ALARM_EN);
   WRITE_PERI_REG(TIMG_T0CONFIG_REG(DEMIURGE_TIMER_GROUP),
                  (1 << TIMG_T0_DIVIDER_S ) | TIMG_T0_EN | TIMG_T0_AUTORELOAD | TIMG_T0_INCREASE | TIMG_T0_ALARM_EN);
}

void IRAM_ATTR startInfiniteTask(void *parameter) {
   ESP_LOGE("MAIN", "Starting audio algorithm in Core %d", xTaskGetAffinity(nullptr));
   auto *demiurge = static_cast<Demiurge *>(parameter);
   demiurge->initialize();
   initialize_tick_timer();
   while (true) {
      wait_timer_alarm();
      demiurge->tick();
   }

}

#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-pro-type-member-init"

Demiurge::Demiurge() {
   ESP_LOGI(TAG, "Starting Demiurge...\n");
   initializeSinks();
};

void Demiurge::initialize() {
   // Initialize LEDs
   gpio_set_direction(GPIO_NUM_21, GPIO_MODE_OUTPUT);
   gpio_set_direction(GPIO_NUM_22, GPIO_MODE_OUTPUT);
   gpio_set_direction(GPIO_NUM_25, GPIO_MODE_OUTPUT);
   gpio_set_direction(GPIO_NUM_26, GPIO_MODE_OUTPUT);

   _dac = new MCP4822(GPIO_NUM_13, GPIO_NUM_14, GPIO_NUM_15);
//   adc128s102_init(&_adc, _vspi);
}

void Demiurge::startRuntime() {
   if (_started)
      return;
   _started = true;
   // Move to CORE
   TaskHandle_t idleTask = xTaskGetIdleTaskHandle();
   esp_task_wdt_delete(idleTask);
   ESP_LOGI("MAIN", "Executing in Core %d", xTaskGetAffinity(nullptr));
   xTaskCreatePinnedToCore(startInfiniteTask, "Audio", 8192, this, 7, &_taskHandle, 0);
}

void Demiurge::initializeSinks() {
   for (auto &_sink : _sinks) {
      _sink = nullptr;
   }
}

#pragma clang diagnostic pop

Demiurge::~Demiurge() {

   delete _dac;
   spi_bus_remove_device(_vspi);
   spi_bus_remove_device(_hspi);

   esp_timer_stop(_timer);
   esp_timer_delete(_timer);
   free(_config);
}

void Demiurge::registerSink(signal_t *processor) {
   ESP_LOGI("MAIN", "Registering Sink: %x", processor);
   configASSERT(processor != nullptr)
   auto *port = (audio_out_port_t *) processor->data;
   configASSERT(port != nullptr)
   configASSERT(port->position > 0 && port->position <= DEMIURGE_MAX_SINKS)
   _sinks[port->position - 1] = processor;
   ESP_LOGE("MAIN", "Registering Sink: %d", port->position);
}

void Demiurge::unregisterSink(signal_t *processor) {
   ESP_LOGI("MAIN", "Unregistering Sink: %x", processor);
   configASSERT(processor != nullptr)
   auto *port = (audio_out_port_t *) processor->data;
   configASSERT(port != nullptr)
   configASSERT(port->position > 0 && port->position <= DEMIURGE_MAX_SINKS)
   _sinks[port->position - 1] = nullptr;
}

void IRAM_ATTR Demiurge::tick() {
   readGpio();
   readADC();
   timerCounter = timerCounter + 10; // pass along number of microseconds.

   uint32_t out1;
   signal_t *sink1 = _sinks[0];
   if (sink1 != nullptr) {
      out1 = audiooutport_read(sink1, timerCounter);
   }

   uint32_t out2;
   signal_t *sink2 = _sinks[1];
   if (sink2 != nullptr) {
      out2 = audiooutport_read(sink2, timerCounter);
   }
   _dac->setOutput(out1, out2);
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

int32_t IRAM_ATTR *Demiurge::inputs() {
   return _inputs;
}

int32_t IRAM_ATTR *Demiurge::outputs() {
   return _outputs;
}

bool IRAM_ATTR Demiurge::gpio(int pin) {
   return (_gpios & (1 << pin)) != 0;
}

int32_t Demiurge::output1() {
   return _outputs[0];
}

int32_t Demiurge::output2() {
   return _outputs[1];
}

