cmake_minimum_required(VERSION 3.13)

set(PICO_SDK_FETCH_FROM_GIT on)

include(${CMAKE_CURRENT_LIST_DIR}/../pico_sdk_import.cmake)

project(demo)

add_subdirectory(../bno_driver bno055_build)

add_executable(demo demo.cpp)

target_link_libraries(demo c bno055_lib pico_stdlib)

pico_sdk_init()
pico_add_extra_outputs(demo)

pico_enable_stdio_usb(demo 1)
pico_enable_stdio_uart(demo 0)

