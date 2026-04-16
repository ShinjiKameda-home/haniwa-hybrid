cmake_minimum_required(VERSION 3.13)

# Import SDK
include(pico_sdk_import.cmake)

# Using not Pico but Pico 2 (RP2350)
set(PICO_BOARD pico2_w)

project(haniwa_guardian C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

# Define file names
add_executable(haniwa_guardian
    haniwa_main.cpp
    haniwa_monitor.cpp
    haniwa_connector.cpp 
)

# Library link to use blinking LED, AD converter, and WiFi
target_link_libraries(haniwa_guardian
    pico_stdlib
    hardware_adc
    pico_cyw43_arch_lwip_threadsafe_background
)

# Enable USB output, disable UART output
pico_enable_stdio_usb(haniwa_guardian 1)
pico_enable_stdio_uart(haniwa_guardian 0)

# Activate generating .uf2 file
pico_add_extra_outputs(haniwa_guardian)