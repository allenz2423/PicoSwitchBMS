# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.13)

# Pull in the Pico SDK using the PICO_SDK_PATH environment variable
include(pico_sdk_import.cmake)

# Define the project name and languages
project(PicoSwitchBMS C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(PICO_BOARD pico_w)
set(CMAKE_CXX_STANDARD 17)

# Initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# Define your executable
add_executable(PicoSwitchBMS_app
    main.c
)
if (PICO_CYW43_SUPPORTED)
    target_link_libraries(pico_cyw43_arch_none)
endif()

# Link all required libraries: stdlib, i2c, and the Pico W wireless driver
target_link_libraries(PicoSwitchBMS_app PRIVATE
    pico_stdlib
    hardware_i2c
    pico_cyw43_arch_lwip_threadsafe_background
)

# Setup USB stdio for printf output
pico_enable_stdio_usb(PicoSwitchBMS_app 1)
pico_enable_stdio_uart(PicoSwitchBMS_app 0)

# Create the .uf2 file for flashing
pico_add_extra_outputs(PicoSwitchBMS_app)
