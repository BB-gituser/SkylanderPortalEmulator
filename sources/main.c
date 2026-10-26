#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "bsp/board.h"
#include "tusb.h"

#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "tusb.h"

#include "hardware/spi.h"
#include "pico/stdlib.h"
#include "pico/time.h"

#include "ST7735_TFT.h"
#include "hw.h"

#include "ff.h"

#include "../includes/constant.h"
#include "../includes/display.h"
#include "../includes/joystick.h"
#include "../includes/main.h"
#include "../includes/menu.h"
#include "../includes/pins.h"
#include "../includes/sd.h"
#include "../includes/usb.h"
#include "../includes/usb_descriptors.h"
#include "../includes/utils.h"

/* init constants*/
int to_send = 1;

char sense_counter = 0;
Folder *root = NULL;

Slot slots_list[MAX_SKYLANDER_COUNT];
FIL fil_list[MAX_SKYLANDER_COUNT];

int main(void) {
  // init joystick
  gpio_init(PIN_BUTTON);
  gpio_set_dir(PIN_BUTTON, GPIO_IN);
  gpio_pull_up(PIN_BUTTON);

  // config joystick
  adc_init();
  adc_gpio_init(26);
  adc_gpio_init(27);

  // init display
  init_display();

  // init slots_list to full empty

  for (int i = 0; i < MAX_SKYLANDER_COUNT; i++) {
    slots_list[i].FileIndex = -1;
    slots_list[i].statut = EMPTY;
    slots_list[i].fil = NULL;
    slots_list[i].skylander = NULL;
  }

  // led
  const uint LED_PIN = PICO_DEFAULT_LED_PIN;
  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);
  bool led_state = 1;

  // test mount sd card
  FRESULT fr = f_mount(&fs, "0:", 1);
  if (fr != FR_OK) {
    char msg[COLS + 1];
    snprintf(msg, sizeof msg, "f_mount erreur %d", (int)fr);
    show_status("ERROR SD", msg, ST7735_RED);
    while (true)
      sleep_ms(1000);
  } else {
    show_status("SUCCESS SD", "SD mount success", ST7735_GREEN);
    sleep_ms(2000);
    fillScreen(ST7735_BLACK);
  }

  root = scan_sd_root();

  // INIT TINYUSB
  board_init();
  tud_init(0);

  // init_log();
  fillScreen(ST7735_BLACK);
  gpio_put(LED_PIN, 1);

  static uint32_t next_status = 0;
  uint32_t last = 0;
  while (true) {
    tud_task_();
    queue_dequeue();
    uint32_t now = to_ms_since_boot(get_absolute_time());
    if (now >= next_status && queue_is_empty()) {
      send_status();
      next_status = now + 20;
    }
    if (last < now) { app_task(); last = now + FPS; }
  }
}

void app_task() {
  menu_screen_refresh();
}
