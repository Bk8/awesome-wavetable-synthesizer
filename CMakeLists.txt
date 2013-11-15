set(CMAKE_SOURCE_DIR /home/mast/dev/arduino-cmake)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/ArduinoToolchain.cmake)
cmake_minimum_required(VERSION 2.8)

set(PROJECT_NAME awesome_wavetable_synthesizer)
project(${PROJECT_NAME} C CXX)

#find_package(Arduino 1.0 REQUIRED)

include_directories(src)
set(${PROJECT_NAME}_BOARD uno)
set(${PROJECT_NAME}_SRCS src/oscillator.cc src/oscillator.h target_test/Adafruit_MCP4725.cpp target_test/main.cc)
set(${PROJECT_NAME}_PORT /dev/ttyACM0)
generate_arduino_firmware(${PROJECT_NAME})

