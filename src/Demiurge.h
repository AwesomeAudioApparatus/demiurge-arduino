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
#endif

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "Adsr.h"
#include "AudioInPort.h"
#include "AudioOutPort.h"
#include "ControlPair.h"
#include "CvInPort.h"
#include "FixedSignal.h"
#include "GatePort.h"
#include "Inverter.h"
#include "Mixer.h"
#include "OctavePerVolt.h"
#include "Pan.h"
#include "Passthru.h"
#include "Potentiometer.h"
#include "PushButton.h"
#include "Signal.h"
#include "Sine.h"
#include "Sink.h"
#include "SoundProcessor.h"
#include "Threshold.h"
#include "Timing.h"
#include "VCO.h"
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


//   void operator=(Demiurge const &) = delete;

   static double clip(double value) {
      if (value > 10.0) return 10.0;
      if (value < -10.0) return -10.0;
      return value;
   };

   static uint64_t micros();

   void begin();

   void tick();

   void registerSink(Sink *processor);

   void unregisterSink(Sink *processor);

   double *inputs();

   double *outputs();

   void setDAC(int channel, double voltage);

   bool gpio(int i);

   void printReport();
   uint16_t dac1();
   uint16_t dac2();
   double output1();
   double output2();
   uint16_t *rawAdc();

   Timing *timing[5];

private:

   Demiurge();

   ~Demiurge();

   void initializeDacSpi();

   void initializeAdcSpi();

   void initializeConcurrency();

   void initializeSinks();

   Sink *_sinks[DEMIURGE_MAX_SINKS] = {nullptr, nullptr};

   void readADC();
   bool _started;
   uint32_t _gpios;
   double _inputs[8];
   double _output1;
   double _output2;
   uint16_t _dac1;
   uint16_t _dac2;

   double _outputs[2];
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

   MCP4822 *_dac;
   ADC128S102 *_adc;

   void readGpio();
};


#endif
