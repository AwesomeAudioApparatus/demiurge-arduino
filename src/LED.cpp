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

#include "LED.h"
#include "Demiurge.h"
#include <driver/ledc.h>
#include <esp_task.h>
#include <esp_log.h>

LED::LED(int position) {
   configASSERT(position > 0 && position <= 4)
   _data.me = &_signal;
   _data.position = position;
   _signal.read_fn = led_read;
   _signal.data = &_data;
   _data.channel = LED_CHANNEL[position - 1];
   _data.fade_time_ms = 100;

   ledc_timer_config_t timer_config;
   timer_config.timer_num = LED_TIMER[position - 1];
   timer_config.duty_resolution = LEDC_TIMER_13_BIT;
   timer_config.freq_hz = 5000;
   timer_config.speed_mode = LEDC_HIGH_SPEED_MODE;
   esp_err_t err = ledc_timer_config(&timer_config);
   ESP_ERROR_CHECK(err)

   ledc_channel_config_t conf;
   conf.gpio_num = LED_GPIO[position - 1];;
   conf.timer_sel = LED_TIMER[position - 1];
   conf.speed_mode = LEDC_HIGH_SPEED_MODE;
   conf.channel = LED_CHANNEL[position - 1];
   conf.hpoint = 4095;
   conf.intr_type = LEDC_INTR_DISABLE;
   conf.duty = 0;
   ESP_LOGE("LED", "Channel: %d", conf.channel);
   ESP_LOGE("LED", "Timer: %d", conf.timer_sel);
   ESP_LOGE("LED", "GPIO: %d", conf.gpio_num);
   err = ledc_channel_config(&conf);
   ESP_ERROR_CHECK(err)

   ledc_fade_func_install(0);
}

LED::~LED() {
   if (_data.registered) {
      Demiurge::runtime().unregisterSink(&_signal);
      _data.registered = false;
   }
}

void LED::configure(Signal *input) {
   if (!_data.registered) {
      Demiurge::runtime().registerSink(&_signal);
      _data.registered = true;
   }
   _input = input;
   _data.input = &input->_signal;
   _signal.data = &_data;
}

void LED::configure_fade(int fade_in_ms) {
   this->_data.fade_time_ms = fade_in_ms;
}

static void IRAM_ATTR led_set(ledc_channel_t channel, uint32_t duty, int fade_time_ms) {
   ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE, channel, duty, fade_time_ms);
   ledc_fade_start(LEDC_HIGH_SPEED_MODE, channel, LEDC_FADE_NO_WAIT);
};

uint32_t LED::get() {
   return this->_data.duty;
}

void LED::set(uint32_t duty) {
   this->_data.duty = duty;
   led_set(_data.channel, duty, _data.fade_time_ms);
}

float IRAM_ATTR led_read(signal_t *handle, uint64_t time) {
   auto *led = (led_t *) handle->data;
   if (time > handle->last_calc) {
      ESP_LOGE("LED", "Updating");
      handle->last_calc = time;
      signal_t *upstream = led->input;
      float result = upstream->read_fn(upstream, time);
      auto duty = (uint32_t) (result * 819.0f);
      led_set(led->channel, duty, led->fade_time_ms);
      led->duty = duty;
      handle->cached = result;
      return result;
   }
   return handle->cached;
}
