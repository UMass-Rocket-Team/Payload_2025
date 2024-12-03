cmake_minimum_required(VERSION 3.13)

include(../../third-party/pico-sdk/pico_sdk_init.cmake)

project(bmp388)

pico_sdk_init()

add_executable(bmp388 bmp388.cpp)
target_link_libraries(bmp388 c)

target_link_libraries(bmp388 pico_stdlib hardware_i2c)
pico_add_extra_outputs(bmp388)