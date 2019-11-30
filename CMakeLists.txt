cmake_minimum_required(VERSION 3.14)
project(demiurge)
set(CMAKE_CXX_STANDARD 14)

if ("!$ENV{ESP_INCLUDES}" STREQUAL "!")
    set(ESP_INCLUDES "$ENV{HOME}/.arduino15/packages/esp32/hardware/esp32/1.0.4/tools/sdk/include")
else ()
    set(ESP_INCLUDES, "$ENV{ESP_INCLUDES}")
endif ()

INCLUDE_DIRECTORIES(
        ${ESP_INCLUDES}/freertos
        ${ESP_INCLUDES}/newlib
        ${ESP_INCLUDES}/config
        ${ESP_INCLUDES}/esp32
        ${ESP_INCLUDES}/soc
        ${ESP_INCLUDES}/heap
        ${ESP_INCLUDES}/driver
)

add_library(
        demiurge
        src/Adsr.cpp
        src/Adsr.h
        src/AudioInPort.cpp
        src/AudioInPort.h
        src/AudioOutPort.cpp
        src/AudioOutPort.h
        src/ControlPair.cpp
        src/ControlPair.h
        src/CvInPort.cpp
        src/CvInPort.h
        src/Demiurge.cpp
        src/Demiurge.h
        src/FixedSignal.cpp
        src/FixedSignal.h
        src/GatePort.cpp
        src/GatePort.h
        src/Inverter.cpp
        src/Inverter.h
        src/mcp48x2/MCP48x2.cpp
        src/mcp48x2/MCP48x2.h
        src/Mixer.cpp
        src/Mixer.h
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
        src/Sink.cpp
        src/Sink.h
        src/SoundProcessor.cpp
        src/SoundProcessor.h
        src/Threshold.cpp
        src/Threshold.h
        src/VCO.cpp
        src/VCO.h
        src/Volume.cpp
        src/Volume.h
)