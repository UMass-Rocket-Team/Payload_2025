cmake_minimum_required(VERSION 3.13)

set(PICO_SDK_FETCH_FROM_GIT on)

include(${CMAKE_CURRENT_LIST_DIR}/../pico_sdk_import.cmake)

project(bno055)

pico_sdk_init()

add_library(bno055_lib STATIC ./bno055.cpp)

target_include_directories(bno055_lib PUBLIC ./)
target_link_libraries(bno055_lib pico_stdlib hardware_i2c)