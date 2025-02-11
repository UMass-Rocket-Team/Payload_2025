cmake_minimum_required(VERSION 3.13)

# set(PICO_BOARD pico2)
set(PICOTOOL_FETCH_FROM_GIT_PATH ${CMAKE_CURRENT_LIST_DIR}/../../third-party/picotool)
include(${CMAKE_CURRENT_LIST_DIR}/../../third-party/pico-sdk/pico_sdk_init.cmake)

project(demo)

add_subdirectory(../bmp388 bmp388_build)

add_executable(demo demo.cpp)

target_link_libraries(demo c bmp388_lib pico_stdlib)

pico_sdk_init()
pico_add_extra_outputs(demo)

pico_enable_stdio_usb(demo 1)
pico_enable_stdio_uart(demo 0)

