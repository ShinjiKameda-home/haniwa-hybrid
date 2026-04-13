cmake_minimum_required(VERSION 3.13)

# Import SDK
include(pico_sdk_import.cmake)

# Using not Pico but Pico 2 (RP2350)
set(PICO_BOARD pico2)

project(haniwa_monitor C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

# Define file names
add_executable(haniwa_monitor
    haniwa_monitor.cpp 
)

# Library link to use blinking LED
target_link_libraries(haniwa_monitor
    pico_stdlib
    hardware_adc
)

# Enable USB output, disable UART output
pico_enable_stdio_usb(haniwa_monitor 1)
pico_enable_stdio_uart(haniwa_monitor 0)

# Activate generating .uf2 file
pico_add_extra_outputs(haniwa_monitor)