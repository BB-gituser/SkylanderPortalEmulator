#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "tusb.h"
#include "bsp/board.h"

#include "includes/usb_descriptors.h"

#include "hardware/gpio.h"
#include "hardware/adc.h"

#include "pico/stdlib.h"
#include "hardware/spi.h"

#include "ST7735_TFT.h"
#include "hw.h"

#include "ff.h"

#include "includes/main.h"
#include "includes/display.h"
#include "includes/sd.h"
#include "includes/usb.h"
#include "includes/menu.h"
#include "includes/joystick.h"
#include "includes/constant.h"
#include "includes/pins.h"
#include "pico/time.h"

/* init constants*/


FIL *loaded_skylanders[MAX_SKYLANDER_COUNT] = {NULL};

Player players[2];

bool shutthefuckup = false;
bool send_data_now = false;
char sense_counter = 0;

char selected_path[256] = {0};
int menu_ = 0;
int player_selected = 0;

Folder *root = NULL;

int just_add = 0;
int just_remove = 0;

////////////////////////////////////////////////::
int main(void)
{
    // init joystick
    gpio_init(PIN_BUTTON);
    gpio_set_dir(PIN_BUTTON, GPIO_IN);
    gpio_pull_up(PIN_BUTTON);

    // config joystick
    adc_init();
    adc_gpio_init(26); 
    adc_gpio_init(27); 


    //init display
    init_display();
    
    // led
    #ifndef PICO_DEFAULT_LED_PIN
    #define PICO_DEFAULT_LED_PIN 25
    #endif
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    bool led_state = 1;

    //test mount sd card
    FRESULT fr = f_mount(&fs, "0:", 1);
    if (fr != FR_OK) {
        char msg[COLS + 1];
        snprintf(msg, sizeof msg, "f_mount erreur %d", (int)fr);
        show_status("ERROR SD", msg, ST7735_RED);
        while (true) sleep_ms(1000);
    } else {
        show_status("SUCCESS SD", "SD mount success", ST7735_GREEN);
        sleep_ms(2000);
        fillScreen(ST7735_BLACK);
    }

    root =  scan_sd_root();
    
    // INIT TINYUSB
    board_init();
    tud_init(0);

    //init_log();
    fillScreen(ST7735_BLACK);
    gpio_put(LED_PIN, 1);
    uint32_t last = 0;
    while (true) {
        tud_task_();
        uint32_t current_ms = to_ms_since_boot(get_absolute_time());
        if (last < current_ms) {
            app_task();
            last = current_ms + FPS;
        }
    }
}

void app_task() {
    render_menu();
}


