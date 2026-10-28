#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "hardware/adc.h"
#include "hardware/gpio.h"

#include "hardware/spi.h"
#include "pico/stdlib.h"

#include "ST7735_TFT.h"
#include "hw.h"

#include "ff.h"

#include "../includes/constant.h"
#include "../includes/display.h"
#include "../includes/pins.h"

void my_drawText(uint8_t x, uint8_t y, const char *_text, uint16_t color, uint16_t bg, uint8_t size) {
  uint8_t cursor_x, cursor_y;
  uint16_t textsize, i;
  cursor_x = x, cursor_y = y;
  textsize = strlen(_text);
  for(i = 0; i < textsize; i++){
    if(((cursor_x + size * 5) > tft_width) || _text[i] == '\n') {
      cursor_x = 0;
      cursor_y = cursor_y + size * 7 + 3 ;
      if(cursor_y > tft_height) cursor_y = tft_height;
      if(_text[i] == LCD_ASCII_OFFSET) {
        continue;
      }
    }
    if (_text[i] != '\n') drawChar(cursor_x, cursor_y, _text[i], color, bg, size);
    cursor_x = cursor_x + size * 6;
    if(cursor_x > tft_width) {
      cursor_x = tft_width;
    }
  }
}

void init_display(void) {
  spi_init(TFT_SPI, TFT_SPI_BAUD);
  gpio_set_function(TFT_PIN_SCK, GPIO_FUNC_SPI);
  gpio_set_function(TFT_PIN_MOSI, GPIO_FUNC_SPI);
  tft_spi_init();

#if defined(TFT_ENABLE_BLACK)
  TFT_BlackTab_Initialize();
#elif defined(TFT_ENABLE_GREEN)
  TFT_GreenTab_Initialize();
#elif defined(TFT_ENABLE_RED)
  TFT_RedTab_Initialize();
#elif defined(TFT_ENABLE_GENERIC)
  TFT_ST7735B_Initialize();
#endif

  setTextWrap(false);
  fillScreen(ST7735_BLACK);
  setRotation(1);
}

void reset_screen(uint16_t color) { fillScreen(ST7735_BLACK); }

void show_status(const char *title, const char *content, uint16_t color) {
  fillScreen(ST7735_BLACK);
  drawText(16, 8, title, color, ST7735_BLACK, 2);
  if (!content)
    return;
  drawText(0, 24, "----------------------", color, ST7735_BLACK, 1);
  char c[COLS + 1] = {0};
  int y = 0;

  int i = 0;
  int ind = 0;
  
  setTextWrap(true);

  my_drawText(4, 32, content, color, ST7735_BLACK, 1);

  setTextWrap(false);
}

uint16_t rgb888_to_rgb565(uint8_t r, uint8_t g, uint8_t b) {
  uint16_t r5 = (r >> 3) & 0x1F;
  uint16_t g6 = (g >> 2) & 0x3F;
  uint16_t b5 = (b >> 3) & 0x1F;

  return (r5 << 11) | (g6 << 5) | b5;
}

#include "ff.h"

void bmpDraw(uint16_t x, uint16_t y, const char *filename) {
  FIL fil;
  FRESULT res;
  UINT br;
  uint8_t header[54];

  res = f_open(&fil, filename, FA_READ);
  if (res != FR_OK)
    return;

  res = f_read(&fil, header, 54, &br);
  if (res != FR_OK || br != 54 || header[0] != 'B' || header[1] != 'M') {
    f_close(&fil);
    return;
  }

  uint32_t dataOffset = *(uint32_t *)&header[10];
  int32_t width = *(int32_t *)&header[18];
  int32_t height = *(int32_t *)&header[22];
  uint16_t bpp = *(uint16_t *)&header[28];

  if (bpp != 24) {
    f_close(&fil);
    return;
  }

  f_lseek(&fil, dataOffset);

  int row, col;
  uint8_t rgb[3];
  int padding = (4 - (width * 3) % 4) % 4;

  // Lecture de bas en haut (standard BMP)
  for (row = height - 1; row >= 0; row--) {
    for (col = 0; col < width; col++) {
      f_read(&fil, rgb, 3, &br);
      // BMP stocke les couleurs en BGR
      uint16_t color = rgb888_to_rgb565(rgb[2], rgb[1], rgb[0]);
      drawPixel(x + col, y + row, color);
    }
    if (padding > 0) {
      uint8_t pad[3];
      f_read(&fil, pad, padding, &br);
    }
  }

  f_close(&fil);
}
