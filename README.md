# SkylanderPortalEmulator

> This project is a fork of [KAOS](https://github.com/NicoAICP/KAOS) by [NicoAICP](https://github.com/NicoAICP).

<hr>

## SETUP

### SD card content

```
SD
├── Skylanders
│   └── **/*.bin
└── pictures
    ├── air.bmp
    ├── dark.bmp
    ├── earth.bmp
    ├── fire.bmp
    ├── kaos.bmp
    ├── life.bmp
    ├── light.bmp
    ├── magic.bmp
    ├── none.bmp
    ├── tech.bmp
    ├── undead.bmp
    └── water.bmp
```

> **note** : in `**/*.bin` you can have folder whit bin files inside
> **note** : copy the content of sd_template into your sd card


### CLONE AND INITIALIZE THE PROJECT

```sh
git clone https://github.com/BB-gituser/SkylanderPortalEmulator.git
cd SkylanderPortalEmulator
git submodule update --init --recursive
```

### COMPILE THE PROJECT

First, ensure you have the Pico SDK and required build tools installed on your system

```sh
sudo apt update
sudo apt install -y cmake gcc-arm-none-eabi libnewlib-arm-none-eabi build-essential git python3
```

If you haven't set up the Pico SDK yet, you can initialize it with the following commands

```sh
# sample
(
    cd ~
    git clone https://github.com/raspberrypi/pico-sdk.git
    cd pico-sdk
    git submodule update --init
    sed -i '1i #include <cstdint>' ./tools/pioasm/pio_disassembler.h # add missing header
)
export PICO_SDK_PATH=~/pico-sdk
```

Next, copy the configuration and import files into your project workspace

```sh
cp ./includes/tusb_config.h $PICO_SDK_PATH/lib/tinyusb/src/tusb_config.h
# copy pico_sdk_import.cmake
cp $PICO_SDK_PATH/external/pico_sdk_import.cmake .
```

Build the project using CMake and Make

```sh
# SkylanderPortalEmulator/
(
    mkdir build
    cd build
    rm -rf *
    clear && cmake -DCMAKE_BUILD_TYPE=Release -DPICO_BOARD=pico -DTFT_OPTIONS="TFT_ENABLE_RED;TFT_ENABLE_RESET;TFT_ENABLE_TEXT;TFT_ENABLE_SHAPES;TFT_ENABLE_ALL;TFT_ENABLE_ROTATE;TFT_ENABLE_SCROLL;" ..
    make -j$(nproc)
)
```
> **Note:** Remember to replace `TFT_ENABLE_RED` with the color option that matches your specific LCD TFT display.

## Creating the Pico Board

![Connection Diagram](https://raw.githubusercontent.com/BB-gituser/SkylanderPortalEmulator/refs/heads/main/images/create_board.png)

### LCD TFT
- SCK     : GP14 (pin 19)
- SDA     : GP15 (Pin 20)
- A0      : GP10 (Pin 14)
- RESET   : GP11 (Pin 15)
- CS      : GP9 (Pin 12)
- LED     : 3.3V (Pin 36)
- GND     : GND (Pin 38)
- VCC     : 5V (Pin 40)


### SPI SD Card Reader Module

- SD_CS   : GP5 (Pin 7)
- SD_MOSI : GP3 (Pin 5)
- SD_MISO : GP4 (Pin 6)
- SD_SCK  : GP2 (Pin 4)
- GND     : GND (Pin 38)
- VCC     : 5V (Pin 40)

### Joystick

- VCC     : 3.3V (Pin 36)
- GND     : GND (Pin 38)
- VRX     : GP27
- VRY     : GP26
- SW      : GP16

## Features

- Load and emulate Skylanders `.bin` files directly from an SD card
- TFT LCD user interface support
- Joystick navigation support

## IN PROGRESS

- documentation page
- swap force support

## ROADMAP / IDEAS

- [X] joystick support
- [X] TFT LCD support
- [X] PICO-SDK last version support
- [ ] option Menu
- [ ] IR controller support
- [ ] NFC reader support to save physical figurine `.bin` files in the SD card
- [ ] detail menu for `.bin` files
- [ ] edit `bin` files (e.g., modify money, level)

## Usage

*Coming soon.*

## Known Issues

- GUI bugs on refresh

## CHANGELOGS

- **V0.1**
  - implement the TFT LCD
  - implement the Joystick
- **V1.0**
  - load bin files
  - unload bin files

## RESSOURCES

### Skylanders `.bin` files

- [skylandersNFC](https://github.com/skylandersNFC)
- [Skylanders-Ultimate-NFC-Pack](https://github.com/skylandersNFC/Skylanders-Ultimate-NFC-Pack)

### Other skylanders ressources

- [Skylanders ID list](https://github.com/Texthead1/Skylander-IDs)