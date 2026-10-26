# ==============================================================================
# Project Name : SkylandersEmul
# Author       : BB-gituser
# Description: Firmware for Raspberry Pi Pico to emulate Skylanders hardware.
# Version      : 0.1.0
# ==============================================================================


# -- CMake Configuration  --

cmake_minimum_required(VERSION 3.13)

# C and C++ standars

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# -- Pico SDK setup --
include(pico_sdk_import.cmake)
project(SkylandersEmul C CXX ASM)
pico_sdk_init()


# apply sreen options to the compiler
foreach(opt IN LISTS TFT_OPTIONS)
    message(">>> using: ${opt}")
    add_compile_definitions(${opt})
endforeach()


# -- Main Application --

# List of all sources
add_executable(${PROJECT_NAME}
    sources/main.c
    sources/hw_config.c
    sources/display.c
    sources/usb_descriptors.c
    sources/sd.c
    sources/usb.c
    sources/menu.c
    sources/joystick.c
    sources/utils.c
    sources/skylanders.c
    sources/documentation.c
)


# -- External librairies --
add_subdirectory(pico-st7735)
add_subdirectory(no-OS-FatFS-SD-SPI-RPi-Pico/FatFs_SPI fatfs_spi_build)

# -- Link Librairies --
target_link_libraries(${PROJECT_NAME}
    pico_stdlib
    hardware_spi
    st7735
    FatFs_SPI
    hardware_adc
    tinyusb_device
    tinyusb_board
)

# -- Output --
# generate output files
pico_add_extra_outputs(${PROJECT_NAME})