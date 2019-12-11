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

#include "esp_timer.h"
#include "esp_log.h"
#include "Timing.h"
#include "Demiurge.h"

Timing::Timing(const char *name) {
   _started = false;
   _startedAt = 0;
   _stoppedAt = 0;
   _lastInterval = 0;
   _overruns = 0;
   _name = name;
}


bool Timing::started() {
   return _started;
}

bool Timing::start() {
   if (_started) {
      _overruns++;
      return false;
   }
   _started = true;
   _startedAt = esp_timer_get_time();
   return true;
}

void Timing::stop() {
   _stoppedAt = esp_timer_get_time();
   _started = false;
   _lastInterval = _stoppedAt - _startedAt;
}

uint64_t Timing::lastInterval() {
   return _lastInterval;
}

void Timing::report() {
   ESP_LOGD("Timing", "%s  :  %d ", _name, (uint32_t) _lastInterval);
}

