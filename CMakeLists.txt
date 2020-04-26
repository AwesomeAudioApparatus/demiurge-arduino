#
# Copyright 2019, Awesome Audio Apparatus.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

cmake_minimum_required(VERSION 3.14)
project(demiurge)
set(CMAKE_CXX_STANDARD 11)


if ("!$ENV{ESP_INCLUDES}" STREQUAL "!")
    set(ESP_INCLUDES "$ENV{HOME}/.arduino15/packages/esp32/hardware/esp32/1.0.4/tools/sdk/include")
    INCLUDE_DIRECTORIES(
            ${ESP_INCLUDES}/config
            ${ESP_INCLUDES}/driver
            ${ESP_INCLUDES}/esp32
            ${ESP_INCLUDES}/freertos
            ${ESP_INCLUDES}/hal
            ${ESP_INCLUDES}/heap
            ${ESP_INCLUDES}/log
            ${ESP_INCLUDES}/newlib
            ${ESP_INCLUDES}/soc
    )
endif ()


add_library(
        demiurge
        src/adc128s102/ADC128S102.cpp
        src/adc128s102/ADC128S102.h
        src/LED.h
        src/LED.cpp
        src/mcp4822/MCP4822.cpp
        src/mcp4822/MCP4822.h
        src/Adder.cpp
        src/Adder.h
        src/Adsr.cpp
        src/Adsr.h
        src/AudioInPort.cpp
        src/AudioInPort.h
        src/AudioOutPort.cpp
        src/AudioOutPort.h
        src/Averager.cpp
        src/Averager.h
        src/Calculator.cpp
        src/Calculator.h
        src/ControlPair.cpp
        src/ControlPair.h
#        src/Cordic.h
#        src/Cordic.cpp
        src/CvInPort.cpp
        src/CvInPort.h
        src/CvOutPort.cpp
        src/CvOutPort.h
        src/Demiurge.cpp
        src/Demiurge.h
        src/FixedSignal.cpp
        src/FixedSignal.h
        src/GateInPort.cpp
        src/GateInPort.h
        src/GateOutPort.cpp
        src/GateOutPort.h
        src/Inverter.cpp
        src/Inverter.h
        src/Log.cpp
        src/Log.h
        src/Log2.cpp
        src/Log2.h
        src/Log10.cpp
        src/Log10.h
        src/Mixer.cpp
        src/Mixer.h
        src/Multiplier.cpp
        src/Multiplier.h
        src/OctavePerVolt.cpp
        src/OctavePerVolt.h
        src/Pan.cpp
        src/Pan.h
        src/Passthru.cpp
        src/Passthru.h
        src/Potentiometer.cpp
        src/Potentiometer.h
        src/PushButton.cpp
        src/PushButton.h
        src/Signal.cpp
        src/Signal.h
        src/Threshold.cpp
        src/Threshold.h
        src/Oscillator.cpp
        src/Oscillator.h
        src/Volume.cpp
        src/Volume.h
        src/spi/aaa_spi.c
        src/spi/aaa_spi.h
)