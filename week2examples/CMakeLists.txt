cmake_minimum_required(VERSION 3.20)

include(${CMAKE_SOURCE_DIR}/cmake/PlatformIO.cmake)

set(WEEK2_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

add_platformio_environment(
  TARGET_NAME week2_esp
  PROJECT_DIR ${WEEK2_ROOT}/esp
  ENV_NAME lilygo-t-display
  TARGET_FOLDER "Firmware/Week2/ESP"
)

add_platformio_environment(
  TARGET_NAME week2_nano
  PROJECT_DIR ${WEEK2_ROOT}/nano
  ENV_NAME nano33ble
  TARGET_FOLDER "Firmware/Week2/Nano"
)

add_platformio_environment(
  TARGET_NAME week2_pico
  PROJECT_DIR ${WEEK2_ROOT}/pico
  ENV_NAME pico
  TARGET_FOLDER "Firmware/Week2/Pico"
)
