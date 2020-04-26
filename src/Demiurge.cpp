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

#include <esp_log.h>
#include <esp_task_wdt.h>
#include <soc/timer_group_reg.h>
#include <driver/ledc.h>

#include "Demiurge.h"

#define TAG "DEMI"
#define DEMIURGE_TIMER_GROUP 0

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
#pragma ide diagnostic ignored "hicpp-signed-bitwise"

static void IRAM_ATTR initialize_tick_timer(int speed) {
   uint16_t micros_per_tick = 1000000 / speed;
   WRITE_PERI_REG(TIMG_T0CONFIG_REG(DEMIURGE_TIMER_GROUP),
                  (1 << TIMG_T0_DIVIDER_S) | TIMG_T0_EN | TIMG_T0_AUTORELOAD | TIMG_T0_INCREASE | TIMG_T0_ALARM_EN);

   WRITE_PERI_REG(TIMG_T0LOADHI_REG(DEMIURGE_TIMER_GROUP), 0);
   WRITE_PERI_REG(TIMG_T0LOADLO_REG(DEMIURGE_TIMER_GROUP), 0);
   WRITE_PERI_REG(TIMG_T0ALARMHI_REG(DEMIURGE_TIMER_GROUP), 0);
   WRITE_PERI_REG(TIMG_T0ALARMLO_REG(DEMIURGE_TIMER_GROUP), 40 * micros_per_tick);

   WRITE_PERI_REG(TIMG_T0LOAD_REG(DEMIURGE_TIMER_GROUP), 1);
}

static void IRAM_ATTR wait_timer_alarm() {
   while (READ_PERI_REG(TIMG_T0CONFIG_REG(DEMIURGE_TIMER_GROUP)) & TIMG_T0_ALARM_EN);
   WRITE_PERI_REG(TIMG_T0CONFIG_REG(DEMIURGE_TIMER_GROUP),
                  (1 << TIMG_T0_DIVIDER_S) | TIMG_T0_EN | TIMG_T0_AUTORELOAD | TIMG_T0_INCREASE | TIMG_T0_ALARM_EN);
}

void IRAM_ATTR startInfiniteTask(void *parameter) {
   ESP_LOGD("MAIN", "Starting audio algorithm in Core %d", xTaskGetAffinity(nullptr));
   auto *demiurge = static_cast<Demiurge *>(parameter);
   demiurge->initialize();
   initialize_tick_timer(demiurge->_computes_per_second);
   while (true) {
      wait_timer_alarm();
      demiurge->tick();
      if (READ_PERI_REG(TIMG_T0CONFIG_REG(DEMIURGE_TIMER_GROUP)) & TIMG_T0_ALARM_EN)
         demiurge->overrun++;
   }

}

#pragma clang diagnostic pop


Demiurge::Demiurge() {
   ESP_LOGI(TAG, "Starting Demiurge...\n");
   for( int i=0 ; i < DEMIURGE_MAX_SINKS; i++ )
      _sinks[i] = nullptr;
   _started = false;
   _gpios = 0;
   initializeSinks();
};

static gpio_num_t gpio_output[] = {GPIO_NUM_21, GPIO_NUM_22, GPIO_NUM_25, GPIO_NUM_26, GPIO_NUM_33};
static uint32_t gpio_output_level[] = {1, 1, 1, 1, 0};
static gpio_num_t gpio_input[] = {GPIO_NUM_32, GPIO_NUM_36, GPIO_NUM_37, GPIO_NUM_38, GPIO_NUM_39};

void Demiurge::initialize() {
   for (int i = 0; i < sizeof(gpio_output) / sizeof(gpio_num_t); i++) {
      ESP_LOGI("Demiurge", "Init GPIO%u", gpio_output[i]);
      esp_err_t error = gpio_set_direction(gpio_output[i], GPIO_MODE_OUTPUT);
      configASSERT(error == ESP_OK)
      gpio_set_level(gpio_output[i], gpio_output_level[i]);
   }

   for (int i = 0; i < sizeof(gpio_input) / sizeof(gpio_num_t); i++) {
      ESP_LOGI("Demiurge", "Init GPIO%u", gpio_input[i]);
      esp_err_t error = gpio_set_direction(gpio_input[i], GPIO_MODE_INPUT);
      configASSERT(error == ESP_OK)
   }
   ESP_LOGI("Demiurge", "Initialized GPIO done");

   _dac = new MCP4822(GPIO_NUM_13, GPIO_NUM_14, GPIO_NUM_15);
   _adc = new ADC128S102(GPIO_NUM_23, GPIO_NUM_19, GPIO_NUM_18, GPIO_NUM_5);
}

void Demiurge::startRuntime(int speed) {
   if (_started)
      return;
   _started = true;
   _computes_per_second = speed;
   TaskHandle_t idleTask = xTaskGetIdleTaskHandle();
   esp_task_wdt_delete(idleTask);
   ESP_LOGI("MAIN", "Executing in Core %d", xTaskGetAffinity(nullptr));
   xTaskCreatePinnedToCore(startInfiniteTask, "Audio", 8192, this, 7, &_taskHandle, 0);
}

void Demiurge::initializeSinks() {
   ESP_LOGI("MAIN", "Clearing all sinks!");
   for (auto &_sink : _sinks) {
      _sink = nullptr;
   }
}

Demiurge::~Demiurge() {
   delete _dac;
   delete _adc;
}

void Demiurge::registerSink(signal_t *processor) {
   ESP_LOGI("MAIN", "Registering Sink: %llx", (uint64_t) processor);
   configASSERT(processor != nullptr)
   for( int i=0; i < DEMIURGE_MAX_SINKS; i++ ){
      if( _sinks[i] == nullptr ){
         _sinks[i] = processor;
         ESP_LOGI("MAIN", "Registering Sink: %d", i);
         break;
      }
   }
}

void Demiurge::unregisterSink(signal_t *processor) {
   ESP_LOGI("MAIN", "Unregistering Sink: %llx", (uint64_t) processor);
   configASSERT(processor != nullptr)
   for( int i=0; i < DEMIURGE_MAX_SINKS; i++ ){
      if( _sinks[i] == processor ){
         _sinks[i] = nullptr;
         ESP_LOGI("MAIN", "Unregistering Sink: %d, %llx", i, (uint64_t) processor);
         break;
      }
   }
}

void IRAM_ATTR Demiurge::tick() {
   readGpio();
   readADC();
   timerCounter = timerCounter + 10; // pass along number of microseconds.

   float out1 = 0.0;
   signal_t *sink1 = _sinks[0];
   if (sink1 != nullptr) {
      out1 = audiooutport_read(sink1, timerCounter);
   }

   float out2 = 0.0;
   signal_t *sink2 = _sinks[1];
   if (sink2 != nullptr) {
      out2 = audiooutport_read(sink2, timerCounter);
   }
   _dac->setOutput((uint16_t) ((10.0f - out1) * 204.8f), (uint16_t) ((10.0f - out2) * 204.8f));
   _outputs[0] = out1;
   _outputs[1] = out2;
}


void IRAM_ATTR Demiurge::readADC() {
   // Scale inputs and put in the right order. 0-3=Jacks, 4-7=Pots, in Volts.
   uint8_t buf[20];
   _adc->copy_buffer(buf);

   _inputs[3] = -(((buf[0] << 8) + buf[1]) / 204.8f - 10.0f);
   _inputs[2] = -(((buf[2] << 8) + buf[3]) / 204.8f - 10.0f);
   _inputs[1] = -(((buf[4] << 8) + buf[5]) / 204.8f - 10.0f);
   _inputs[0] = -(((buf[6] << 8) + buf[7]) / 204.8f - 10.0f);
   _inputs[7] = -(((buf[8] << 8) + buf[9]) / 204.8f - 10.0f);
   _inputs[6] = -(((buf[10] << 8) + buf[11]) / 204.8f - 10.0f);
   _inputs[5] = -(((buf[12] << 8) + buf[13]) / 204.8f - 10.0f);
   _inputs[4] = -(((buf[14] << 8) + buf[15]) / 204.8f - 10.0f);
//   for (int i = 0; i < 4; i++) {
//      _inputs[3 - i] = -(buf[i*2]+buf[i*2+1]) / 204.8f - 10.0f;
//   }
//   for (int i = 4; i < 8; i++) {
//      _inputs[11 - i] = -(buf[i*2]+buf[i*2+1]) / 204.8f - 10.0f;
//   }
}

void IRAM_ATTR Demiurge::readGpio() {
   _gpios = gpio_input_get() | (((uint64_t) gpio_input_get_high()) << 32);
}

float IRAM_ATTR Demiurge::input(int number) {
   configASSERT(number > 0 && number <= 8)
   return _inputs[number - 1];
}

float IRAM_ATTR Demiurge::output(int number) {
   configASSERT(number > 0 && number <= 2)
   return _outputs[number - 1];
}

static int count = 4;

bool IRAM_ATTR Demiurge::gpio(int pin) {
   return (_gpios >> pin & 1) != 0;
}

void IRAM_ATTR Demiurge::print_adc_buffer(void *dest) {
   _adc->copy_buffer(dest);
}


#undef TAG