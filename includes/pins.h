#ifndef PINS_H
#define PINS_H

// LCD TFT
#define TFT_SPI             spi1
#define TFT_SPI_BAUD        (8 * 1000 * 1000)
#define TFT_PIN_SCK         14
#define TFT_PIN_MOSI        15

// JOYSTICK
#define PIN_X               26                 // GP26 / ADC0
#define PIN_Y               27                 // GP27 / ADC1
#define PIN_BUTTON          16                 // GP16

#endif