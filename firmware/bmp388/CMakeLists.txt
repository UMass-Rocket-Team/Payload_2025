cmake_minimum_required(VERSION 3.13)

set(PICOTOOL_FETCH_FROM_GIT_PATH ${CMAKE_CURRENT_LIST_DIR}/../../third-party/picotool)

include(${CMAKE_CURRENT_LIST_DIR}/../../third-party/pico-sdk/pico_sdk_init.cmake)

project(bmp388)

pico_sdk_init()

# add_executable(bmp388 bmp388.cpp)
add_library(bmp388_lib STATIC ./bmp388.cpp)
target_include_directories(bmp388_lib PUBLIC ./)

target_link_libraries(bmp388_lib pico_stdlib hardware_i2c)
# pico_add_extra_outputs(bmp388)