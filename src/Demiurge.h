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
#include "Oscillator.h"
#include "adc128s102/ADC128S102.h"
#include "mcp4822/MCP4822.h"

#define DEMIURGE_MAX_SINKS 2

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

   bool gpio(int i);

   void initialize();

   MCP4822 *_dac;
   ADC128S102 *_adc;


private:

   Demiurge();

   ~Demiurge();

   void initializeSinks();

   signal_t *_sinks[DEMIURGE_MAX_SINKS] = {nullptr, nullptr};

   void readADC();
   bool _started;
   uint32_t _gpios;
   int32_t _inputs[8];

   int32_t _outputs[2];
   TaskHandle_t _taskHandle;

   uint64_t timerCounter;         // in microseconds, increments 50 at a time.

   esp_timer_create_args_t *_config;
   esp_timer_handle_t _timer;

   void readGpio();
};


#endif
