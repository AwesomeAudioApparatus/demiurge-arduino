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

#include <stdint.h>
#include <freertos/FreeRTOS.h>

#include "freertos/task.h"
#include "Adsr.h"
#include "AudioInPort.h"
#include "AudioOutPort.h"
#include "ControlPair.h"
#include "CvInPort.h"
#include "FixedSignal.h"
#include "GateInPort.h"
#include "Inverter.h"
#include "LED.h"
#include "Mixer.h"
#include "OctavePerVolt.h"
#include "Pan.h"
#include "Passthru.h"
#include "Potentiometer.h"
#include "PushButton.h"
#include "Signal.h"
#include "Threshold.h"
#include "Oscillator.h"
#include "adc128s102/ADC128S102.h"
#include "mcp4822/MCP4822.h"

#ifndef DEMIURGE_SAMPLE_TIME
#define DEMIURGE_SAMPLE_TIME 20     // Number of microseconds per tick(), a.k.a sample time
#endif
#define DEMIURGE_MAX_SINKS 2

class Demiurge {

public:

   static Demiurge &runtime() {

      static Demiurge instance;
      return instance;
   }

   static void begin() {
      Demiurge::runtime().startRuntime();
   }

   void operator=(Demiurge const &) = delete;

   static float clipGate(float value) {
      if (value >= 0.9) return 1.0;
      return 0.0;
   }

   static float clipCV(float value) {
      if (value >= 5.0) return 5.0;
      if (value <= -5.0) return -5.0;
      return value;
   }

   static float clipAudio(float value) {
      if (value >= 10.0) return 10.0;
      if (value <= -10.0) return -10.0;
      return value;
   };

   void startRuntime();

   void tick();

   void registerSink(signal_t *processor);

   void unregisterSink(signal_t *processor);

   float input(int number);

   float output(int number);

   bool gpio(int i);

   // initialize() is for internal use only, and must not be called by application
   void initialize();

   // Overrun increments means that the tick() took longer than the sample time.
   // If you see this happening, either increase the ${DEMIURGE_SAMPLE_TIME} (default=10) or optimize the tick()
   // evaluation to take less time.
   uint32_t overrun = 0;

   void print_adc_buffer(void *dest);

   uint64_t _gpios;

private:

   Demiurge();

   ~Demiurge();

   void readADC();

   void initializeSinks();

   void readGpio();

   signal_t *_sinks[DEMIURGE_MAX_SINKS] = {nullptr, nullptr};
   bool _started;
   float _inputs[8] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
   float _outputs[2] = {0.0f, 0.0f};
   TaskHandle_t _taskHandle = nullptr;
   uint64_t timerCounter = 0;         // in microseconds, increments 50 at a time.
   MCP4822 *_dac = nullptr;
   ADC128S102 *_adc = nullptr;
};


#endif
