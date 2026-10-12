#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include "ST7735_TFT.h"

void init_display(void);
void show_status(const char *title, const char *content, uint16_t color);


void init_log();
void put_log(const char *line, uint16_t color);
#endif