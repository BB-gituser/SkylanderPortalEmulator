#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include "ST7735_TFT.h"

void init_display(void);
void show_status(const char *title, const char *content, uint16_t color);
void reset_screen(uint16_t color);

uint16_t rgb888_to_rgb565(uint8_t r, uint8_t g, uint8_t b);
void bmpDraw(uint16_t x, uint16_t y, const char *filename);
#endif