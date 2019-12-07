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

#include "Inverter.h"
#include "esp_system.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#pragma ide diagnostic ignored "cppcoreguidelines-pro-type-member-init"

Inverter::Inverter() {
   _midpoint = 0.0;
   _scale = 1.0;
}

Inverter::Inverter(float midpoint) {
   _midpoint = midpoint;
   _scale = 1.0;
}

Inverter::Inverter(float midpoint, float scale) {
   _midpoint = midpoint;
   _scale = scale;
}

Inverter::~Inverter() = default;

void Inverter::configure(Signal *input) {
   _input = input;
}

float IRAM_ATTR Inverter::update(uint64_t time) {
   return 0;
}

#pragma clang diagnostic pop
