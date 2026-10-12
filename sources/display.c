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

#include "../includes/display.h"
#include "../includes/constant.h"
#include "../includes/pins.h"

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

void show_status(const char *title, const char *content, uint16_t color) {
  fillScreen(ST7735_BLACK);
  drawText(16, 8, title, color, ST7735_BLACK, 2);
  if (!content)
    return;
  drawText(0, 24, "----------------------", color, ST7735_BLACK, 1);
  drawText(4, 32, content, color, ST7735_BLACK, 1);
}

// log

int log_line;

void init_log() {
  fillScreen(ST7735_BLACK);
  uint16_t color = ST7735_BLUE;
  log_line = 0;
  drawText(16, 8, "LOGS", color, ST7735_BLACK, 2);
  drawText(0, 24, "----------------------", color, ST7735_BLACK, 1);
}

void put_log(const char *line, uint16_t color) {
  drawText(4, 32 + log_line * 8, line, color, ST7735_BLACK, 1);
  log_line++;
}