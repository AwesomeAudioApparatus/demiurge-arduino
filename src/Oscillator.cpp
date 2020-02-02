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

#include <math.h>
#include <esp_task.h>
#include <esp_log.h>
#include "Demiurge.h"
#include "Cordic.h"

static bool sine_wave_initialized = false;
static float sine_wave[SINEWAVE_SAMPLES];

Oscillator::Oscillator(int mode) {
   ESP_LOGE("Oscillator", "Constructor: %x", (void *) this );
   if( ! sine_wave_initialized )
   {
      for(int i=0; i < SINEWAVE_SAMPLES; i++){
         double radians = ((double) i/360) * M_TWOPI;
         sine_wave[i] = sin(radians);
      }
   }
   _signal.read_fn = oscillator_read;
   _data.mode = mode;
   _triggerControl = nullptr;
   _amplitudeControl = nullptr;
   _frequencyControl = nullptr;
   _signal.data = &_data;
   _data.lastTrig = 0;
}

Oscillator::~Oscillator() = default;

void Oscillator::configure(Signal *freqControl, Signal *amplitudeControl, Signal *trigControl) {
   configureFrequency(freqControl);
   configureAmplitude(amplitudeControl);
   configureTrig(trigControl);
}

void Oscillator::configureFrequency(Signal *freqControl) {
   configASSERT(freqControl != nullptr)
   ESP_LOGE("Oscillator", "Configure frequency control: %x for %x", freqControl, (void *) this );

   _frequencyControl = freqControl;
   _data.frequency = &freqControl->_signal;
   ESP_LOGE("Oscillator", "frequency controller: %x", freqControl->_signal );
}

void Oscillator::configureAmplitude(Signal *amplitudeControl) {
   configASSERT(amplitudeControl != nullptr)
   ESP_LOGE("Oscillator", "Configure amplitude control: %x for %x", amplitudeControl, (void *) this );
   _amplitudeControl = amplitudeControl;
   _data.amplitude = &amplitudeControl->_signal;
   ESP_LOGE("Oscillator", "amplitude controller: %x", amplitudeControl->_signal );
}

void Oscillator::configureTrig(Signal *trigControl) {
   configASSERT(trigControl != nullptr)
   _triggerControl = trigControl;
   _data.trigger = &trigControl->_signal;
}

float IRAM_ATTR oscillator_read(signal_t *handle, uint64_t time_in_us) {
   // time in microseconds
   auto *osc = (oscillator_t *) handle->data;
   if (time_in_us > handle->last_calc) {
      handle->last_calc = time_in_us;
      signal_t *freqControl = osc->frequency;
      float freq=440;
      if (freqControl != nullptr) {
         float voltage = freqControl->read_fn(freqControl, time_in_us);
         freq = octave_frequencyOf(voltage);
         handle->extra1 = voltage;
         handle->extra2 = freq;
      }
      uint32_t period_in_us = 1000000 / freq;

      float amplitude = 1.0f;
      if (osc->amplitude != nullptr) {
//         amplitude = (float) osc->amplitude->read_fn(osc->amplitude, time_in_us);
      }

      switch (osc->mode) {
         case DEMIURGE_SINE: {
            uint32_t x = time_in_us % period_in_us;  // gives the index within a period, as microseconds.
            double percentage_of_cycle = ((double) x) / period_in_us;
            uint16_t index = SINEWAVE_SAMPLES * percentage_of_cycle;
            double out = sine_wave[index] * amplitude;
            handle->cached = out;
            return out;
         }
         case DEMIURGE_SQUARE: {
            uint64_t x = time_in_us % period_in_us;
            double out;
            if( x > period_in_us / 2)
               out = amplitude;
            else
               out = -amplitude;
            handle->cached = out;
            return out;
         }
         case DEMIURGE_TRIANGLE: {
            // TODO
            break;
         }
         case DEMIURGE_SAW: {
            uint64_t x = time_in_us % period_in_us;
            double slope = 1.0 / period_in_us;
            auto out = (float) ((slope * x) ) * amplitude;
            handle->cached = out;
            return out;
         }
      }
      return 0.0;
   }
   return handle->cached;
}
