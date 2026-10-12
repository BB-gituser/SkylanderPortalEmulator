cmake_minimum_required(VERSION 3.13)

set(PICO_BOARD pico CACHE STRING "Board type")   # "pico_w" fonctionne aussi (sans LED)

# pico_sdk_import.cmake : a copier depuis  <pico-sdk>/external/pico_sdk_import.cmake
include(pico_sdk_import.cmake)

project(skylanders_portal C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

pico_sdk_init()

# Pilote carte SD (SPI) + FatFs :  make deps  (ou git clone a la main dans lib/)
#   https://github.com/carlk3/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico
add_subdirectory(lib/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src build_sdlib)

add_executable(skylanders_portal skylanders_portal.c)

# tusb_config.h est dans ce dossier
target_include_directories(skylanders_portal PRIVATE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(skylanders_portal PRIVATE
    pico_stdlib
    pico_multicore
    tinyusb_device
    no-OS-FatFS-SD-SDIO-SPI-RPi-Pico
)

# Le port USB appartient au portail : pas de stdio USB.
# Logs et console de commandes sur UART0 (GP0 = TX, GP1 = RX, 115200 bauds).
pico_enable_stdio_usb(skylanders_portal 0)
pico_enable_stdio_uart(skylanders_portal 1)

pico_add_extra_outputs(skylanders_portal)   # genere le .uf2