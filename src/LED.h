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

#ifndef _DEMIURGE_LED_H_
#define _DEMIURGE_LED_H_


#include <driver/gpio.h>
#include <driver/ledc.h>
#include "Signal.h"

const int LED_GPIO[] = {GPIO_NUM_21, GPIO_NUM_22, GPIO_NUM_25, GPIO_NUM_26};
const ledc_timer_t LED_TIMER[] = {LEDC_TIMER_0, LEDC_TIMER_1, LEDC_TIMER_2, LEDC_TIMER_3};
const ledc_channel_t LED_CHANNEL[] = {LEDC_CHANNEL_0, LEDC_CHANNEL_1, LEDC_CHANNEL_2, LEDC_CHANNEL_3};

typedef struct {
   int position;
   signal_t *me;
   signal_t *input;
   int fade_time_ms;
   bool registered;
   ledc_channel_t channel;
   uint32_t duty;
} led_t;

float led_read(signal_t *handle, uint64_t time);

class LED : public Signal {

public:
   explicit LED(int position);

   ~LED() override;

   void configure(Signal *input);

   void configure_fade( int fade_in_ms );

   led_t _data{};

   void set(uint32_t duty);

   uint32_t get();

private:

   Signal *_input{};
};

#endif
