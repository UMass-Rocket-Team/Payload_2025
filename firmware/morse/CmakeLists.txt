cmake_minimum_required(VERSION 3.10)

# Set the project name
project(morse)

# Add the library
add_library(morse morse.c morse.h)

target_sources(morse PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/morse.c)

# Specify include directories for the library
target_include_directories(morse PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})