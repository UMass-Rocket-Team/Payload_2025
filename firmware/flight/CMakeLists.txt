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
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico2 CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(flight C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(flight main.cc datalog.cc DS1307.c)

pico_set_program_name(flight "flight")
pico_set_program_version(flight "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(flight 0)
pico_enable_stdio_usb(flight 1)

# Add the standard library to the build
target_link_libraries(flight
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(flight PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

# Add SD library
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../sd/ sd_library)
target_include_directories(flight PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../sd/src/)

# Add morse library
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../morse/ morse)

# Add PWM library
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../pwm/ pwm_generic)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../bmp388 bmp388_build)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../bno055/bno_driver bno055_build)

# Add scheduler library
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../schedule/ scheduler)
target_include_directories(flight PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../schedule/)

# Set task count for the schedulers 
target_compile_definitions(scheduler PUBLIC SCHEDULER_CORE0_TASK_CNT=3)
target_compile_definitions(scheduler PUBLIC SCHEDULER_CORE1_TASK_CNT=0)

# Enable debugging for the scheduler
target_compile_definitions(scheduler PUBLIC SCHEDULER_DEBUG)

# Add any user requested libraries
target_link_libraries(flight 
        hardware_spi
        hardware_i2c
        hardware_adc
        sd_library
        morse
        pwm_generic
        scheduler
        bmp388_lib
        bno055_lib
        )

pico_add_extra_outputs(flight)

