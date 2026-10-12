cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Pico SDK (PICO_SDK_PATH doit etre defini, ou pico_sdk_import.cmake recupere le SDK)
include(pico_sdk_import.cmake)

project(SkylandersEmul C CXX ASM)

pico_sdk_init()

# ---------------------------------------------------------------------------
# Config de la lib pico-st7735 (valeurs par defaut = vos branchements)
#   CS=GP9  DC(A0)=GP10  RST=GP11, ecran sur spi1 (SCK=GP14, SDA=GP15)
# Surchargeable en ligne de commande, par ex. :
#   cmake .. -DTFT_OPTIONS="TFT_ENABLE_GREEN;TFT_ENABLE_RESET;TFT_ENABLE_TEXT;TFT_ENABLE_SHAPES"
# (si le dossier build existe deja avec d'autres options : le supprimer)
# ---------------------------------------------------------------------------
set(SPI_TFT_PORT "spi1" CACHE STRING "TFT spi port-name")
set(SPI_TFT_CS   "9"    CACHE STRING "TFT CS pin number")
set(SPI_TFT_DC   "10"   CACHE STRING "TFT DC pin number")
set(SPI_TFT_RST  "11"   CACHE STRING "TFT RST pin number")
set(TFT_OPTIONS TFT_ENABLE_GREEN TFT_ENABLE_RESET TFT_ENABLE_TEXT TFT_ENABLE_SHAPES
    CACHE STRING "TFT options/functions")

# Les options doivent aussi etre definies pour notre propre code
# (elles conditionnent les prototypes dans ST7735_TFT.h)
foreach(opt IN LISTS TFT_OPTIONS)
    message(">>> using: ${opt}")
    add_compile_definitions(${opt})
endforeach()

# ---------------------------------------------------------------------------
add_executable(${PROJECT_NAME}
    main.c
    sources/hw_config.c
    sources/display.c
    sources/usb_descriptors.c
    sources/sd.c
    sources/usb.c
    sources/menu.c
    sources/joystick.c
    sources/utils.c
    sources/skylanders.c
)



# Libs (sous-modules git)
add_subdirectory(pico-st7735)
add_subdirectory(no-OS-FatFS-SD-SPI-RPi-Pico/FatFs_SPI fatfs_spi_build)

target_link_libraries(${PROJECT_NAME}
    pico_stdlib
    hardware_spi
    st7735
    FatFs_SPI
    hardware_adc
    tinyusb_device
    tinyusb_board
)

pico_add_extra_outputs(${PROJECT_NAME})