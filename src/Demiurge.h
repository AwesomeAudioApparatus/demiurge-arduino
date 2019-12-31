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

#ifndef _DEMIURGE_DEMIURGE_H_
#define _DEMIURGE_DEMIURGE_H_

#ifndef DEMIURGE_MAX_SINKS
#define DEMIURGE_MAX_SINKS 2
#endif

#ifndef DEMIURGE_VMOSI_PIN
#define DEMIURGE_VMOSI_PIN 23
#endif

#ifndef DEMIURGE_VMISO_PIN
#define DEMIURGE_VMISO_PIN 19
#endif

#ifndef DEMIURGE_VCLK_PIN
#define DEMIURGE_VCLK_PIN 18
#endif

#ifndef DEMIURGE_VCS_PIN
#define DEMIURGE_VCS_PIN 5
#endif

#ifndef DEMIURGE_HMISO_PIN
#define DEMIURGE_HMISO_PIN 12
#endif

#ifndef DEMIURGE_HMOSI_PIN
#define DEMIURGE_HMOSI_PIN 13
#endif

#ifndef DEMIURGE_HCLK_PIN
#define DEMIURGE_HCLK_PIN 14
#endif

#ifndef DEMIURGE_HCS_PIN
#define DEMIURGE_HCS_PIN 15
static const char *const TAG = "Demiurge";
#endif

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "Adsr.h"
#include "AudioInPort.h"
#include "AudioOutPort.h"
#include "ControlPair.h"
#include "CvInPort.h"
#include "FixedSignal.h"
#include "GateInPort.h"
#include "Inverter.h"
#include "Mixer.h"
#include "OctavePerVolt.h"
#include "Pan.h"
#include "Passthru.h"
#include "Potentiometer.h"
#include "PushButton.h"
#include "Signal.h"
#include <stdint.h>
#include "Threshold.h"
#include "Timing.h"
#include "Oscillator.h"
#include "adc128s102/ADC128S102.h"
#include "mcp4822/MCP4822.h"
#include "Timing.h"

class Demiurge {

//   Demiurge(Demiurge const &) = delete;

public:

   static Demiurge &runtime() {

      static Demiurge instance;
      return instance;
   }

   static void begin() {
      Demiurge::runtime().startRuntime();
   }

   void operator=(Demiurge const &) = delete;

   static int32_t clip(int32_t value) {
      if (value >= 0x00800000) return 0x007FFFFF;
      if (value <= -0x00800000) return -0x007FFFFF;
      return value;
   };

   void startRuntime();

   void tick();

   void registerSink(signal_t *processor);

   void unregisterSink(signal_t *processor);

   int32_t *inputs();

   int32_t *outputs();

   void setDAC(int channel, int32_t voltage);

   bool gpio(int i);

   void printReport();
   uint16_t dac1();
   uint16_t dac2();
   int32_t output1();
   int32_t output2();
   uint16_t *rawAdc();

   Timing *timing[5];

   void initialize();

private:

   Demiurge();

   ~Demiurge();

   void initializeDacSpi();

   void initializeAdcSpi();

   void initializeConcurrency();
   void initializeHardware();

   void initializeSinks();

   signal_t *_sinks[DEMIURGE_MAX_SINKS] = {nullptr, nullptr};

   void readADC();
   bool _started;
   uint32_t _gpios;
   int32_t _inputs[8];
   int32_t _output1;
   int32_t _output2;
   uint16_t _dac1;
   uint16_t _dac2;

   int32_t _outputs[2];
   uint64_t _startTime;
   uint64_t _lastMeasure;
   volatile bool _enterred;
   volatile uint32_t _timerruns;
   volatile uint32_t _overruns;
   TaskHandle_t _taskHandle;

   uint64_t timerCounter;         // in microseconds, increments 50 at a time.

   esp_timer_create_args_t *_config;
   esp_timer_handle_t _timer;

   spi_device_handle_t _hspi;
   spi_bus_config_t _hspiBusConfig;
   spi_device_interface_config_t _hspiDeviceIntfConfig;

   spi_device_handle_t _vspi;
   spi_bus_config_t _vspiBusConfig;
   spi_device_interface_config_t _vspiDeviceIntfConfig;

   mcp4822 _dac;
   adc128s102 _adc;

   void readGpio();
};


#endif
