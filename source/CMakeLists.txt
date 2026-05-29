cmake_minimum_required(VERSION 3.20)

include(${CMAKE_SOURCE_DIR}/cmake/PlatformIO.cmake)

set(SOURCE_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

add_platformio_environment(
  TARGET_NAME source_esp
  PROJECT_DIR ${SOURCE_ROOT}/esp
  ENV_NAME lilygo-t-display
  TARGET_FOLDER "Firmware/Source/ESP"
)

add_platformio_test_environment(
  TARGET_NAME source_esp
  PROJECT_DIR ${SOURCE_ROOT}/esp
  ENV_NAME esp32-test
  TARGET_FOLDER "Firmware/Source/ESP"
)

add_platformio_environment(
  TARGET_NAME source_nano
  PROJECT_DIR ${SOURCE_ROOT}/nano
  ENV_NAME nano33ble
  TARGET_FOLDER "Firmware/Source/Nano"
)

add_platformio_test_environment(
  TARGET_NAME source_nano
  PROJECT_DIR ${SOURCE_ROOT}/nano
  ENV_NAME nano33ble-test
  TARGET_FOLDER "Firmware/Source/Nano"
)

add_platformio_environment(
  TARGET_NAME source_pico
  PROJECT_DIR ${SOURCE_ROOT}/pico
  ENV_NAME pico
  TARGET_FOLDER "Firmware/Source/Pico"
)
