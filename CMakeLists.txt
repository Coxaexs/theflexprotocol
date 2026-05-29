cmake_minimum_required(VERSION 3.20)

project(theflexprotocol NONE)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

option(BUILD_SOURCE_FIRMWARE "Enable source firmware targets" ON)
option(BUILD_WEEK2_EXAMPLES "Enable week2examples firmware targets" OFF)

add_subdirectory(source)

if(BUILD_WEEK2_EXAMPLES)
  add_subdirectory(week2examples)
endif()
