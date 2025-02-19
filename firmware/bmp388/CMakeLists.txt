cmake_minimum_required(VERSION 3.13)

set(PICO_SDK_FETCH_FROM_GIT on)

include(${CMAKE_CURRENT_LIST_DIR}/pico_sdk_import.cmake)

project(bmp388)

pico_sdk_init()

add_library(bmp388_lib STATIC ./bmp388.cpp)

target_include_directories(bmp388_lib PUBLIC ./)
target_link_libraries(bmp388_lib pico_stdlib hardware_i2c)