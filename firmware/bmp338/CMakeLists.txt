cmake_minimum_required(VERSION 3.13)

include(../../third-party/pico-sdk/pico_sdk_init.cmake)

project(bmp338)

pico_sdk_init()

add_executable(bmp338 bmp338.cpp)
target_link_libraries(bmp338 c)

target_link_libraries(bmp338 pico_stdlib hardware_i2c)
pico_add_extra_outputs(bmp338)