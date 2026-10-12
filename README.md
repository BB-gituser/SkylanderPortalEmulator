# SkylanderPortalEmulator

> This project is a fork of [KAOS](https://github.com/NicoAICP/KAOS) by [NicoAICP](https://github.com/NicoAICP)

<hr>

## SETUP

### CLONE AND INIT THE PROJECT

```sh
git clone https://github.com/BB-gituser/SkylanderPortalEmulator.git
cd SkylanderPortalEmulator
git submodule update --init --recursive
```

### COMPILE THE PROJECT

first you need to download PICO SDK

```sh
sudo apt update
sudo apt install -y cmake gcc-arm-none-eabi libnewlib-arm-none-eabi build-essential git python3
```

```sh
# sample
(
    cd ~
    git clone https://github.com/raspberrypi/pico-sdk.git
    cd pico-sdk
    git submodule update --init
    export PICO_SDK_PATH=~/pico-sdk
)
```

```sh
cp ./includes/tusb_config.h $PICO_SDK_PATH/lib/tinyusb/src/tusb_config.h
# copy pico_sdk_import.cmake
cp $PICO_SDK_PATH/external/pico_sdk_import.cmake .
```

compilation

```sh
(
    mkdir build
    cd build
    rm -rf *
    clear && cmake -DCMAKE_BUILD_TYPE=Release -DPICO_BOARD=pico_w -DTFT_OPTIONS="TFT_ENABLE_RED;TFT_ENABLE_RESET;TFT_ENABLE_TEXT;TFT_ENABLE_SHAPES;TFT_ENABLE_ROTATE" .. && clear && make -j$(nproc)
)
# replace TFT_ENABLE_RED by the color of your LCD TFT
```


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

- VCC     : 3.3V (Pin 37)
- GND     : GND (Pin 38)
- VRX     : GP27
- VRY     : GP26
- SW      : GP16

## Features

- load skylanders bin files

## ideas

- [X] use joystick
- [X] use TFT LCD
- [ ] option Menu
- [ ] add IR controller
- [ ] add NFC reader to save real figurines bin files in the sd card
- [ ] detail menu for bin files
- [ ] edit bin files (set money, set level)

## Usage





## Known Issues

- Unload figurines
- Only slot one can be used
- GUI bugs

## CHANGELOGS

1. implement the TFT LCD
2. implement the Joystick

## RESSOURCES

### Skylanders bin files

- [skylandersNFC](https://github.com/skylandersNFC)
- [Skylanders-Ultimate-NFC-Pack](https://github.com/skylandersNFC/Skylanders-Ultimate-NFC-Pack)