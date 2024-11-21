# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.0.0)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.0.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico2 CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(demo_bno055 C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(demo_bno055 demo_bno055.c )

pico_set_program_name(demo_bno055 "demo_bno055")
pico_set_program_version(demo_bno055 "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(demo_bno055 0)
pico_enable_stdio_usb(demo_bno055 1)

# Add the standard library to the build
target_link_libraries(demo_bno055
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(demo_bno055 PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/..
)

# add_subdirectory(../bno055 BNO055)
# target_include_directories(demo_bno055 PUBLIC ../bno055)


add_subdirectory(../morse morse)
# target_include_directories(demo_bno055 PUBLIC ../morse)
# target_link_libraries(demo_bno055 morse)

# Add any user requested libraries
target_link_libraries(demo_bno055 
        hardware_spi
        hardware_i2c
        # morse
        # bno055
        )

pico_add_extra_outputs(demo_bno055)

