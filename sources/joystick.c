#include <stdbool.h>
#include <stdint.h>

#include "hardware/adc.h"
#include "hardware/gpio.h"

#include "hardware/spi.h"
#include "pico/stdlib.h"
#include "pico/time.h"

#include "../includes/joystick.h"
#include "../includes/constant.h"
#include "../includes/pins.h"

uint32_t i = 0;

JoystickStat joystick_get() {
  JoystickStat out;
  if (i + 200 < to_ms_since_boot(get_absolute_time())) {
    i = to_ms_since_boot(get_absolute_time());
    adc_select_input(0);
    int xValue = adc_read();

    adc_select_input(1);
    int yValue = adc_read();

    out.press = (int)(!gpio_get(PIN_BUTTON));

    out.left = (xValue < 1500) ? 1 : 0;
    out.right = (xValue > 2500) ? 1 : 0;
    out.top = (yValue < 1500) ? 1 : 0;
    out.bottom = (yValue > 2500) ? 1 : 0;
    return out;
  } else {
    out.left = 0;
    out.right = 0;
    out.top = 0;
    out.bottom = 0;
    out.press = 0;
    return out;
  }
}