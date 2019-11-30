cmake_minimum_required(VERSION 3.14)
project(demiurge)

set(CMAKE_CXX_STANDARD 14)

add_library(
        demiurge
        src/AudioInPort.cpp
        src/AudioInPort.h
        src/AudioOutPort.cpp
        src/AudioOutPort.h
        src/CvInPort.cpp
        src/CvInPort.h
        src/Demiurge.cpp
        src/Demiurge.h
        src/GatePort.cpp
        src/GatePort.h
        src/Mixer.cpp
        src/Mixer.h
        src/Pan.cpp
        src/Pan.h
        src/Potentiometer.cpp
        src/Potentiometer.h
        src/PushButton.cpp
        src/PushButton.h
        src/Signal.cpp
        src/Signal.h
        src/SoundProcessor.cpp
        src/SoundProcessor.h
        src/Threshold.cpp
        src/Threshold.h
        src/VCO.cpp
        src/VCO.h )