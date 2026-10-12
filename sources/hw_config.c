#include <string.h>

#include "ff.h"
#include "diskio.h"
#include "hw_config.h"
#include "pico/stdlib.h"

static spi_t spis[] = {{
    .hw_inst = spi0,
    .miso_gpio = 4,
    .mosi_gpio = 3,
    .sck_gpio = 2,
    .baud_rate = 12500 * 1000,
}};

// Une entree par lecteur de carte SD
static sd_card_t sd_cards[] = {{.pcName = "0:",
                                .spi = &spis[0],
                                .ss_gpio = 5,
                                .use_card_detect = false,
                                .m_Status = STA_NOINIT}};

size_t sd_get_num(void) { return count_of(sd_cards); }

sd_card_t *sd_get_by_num(size_t num) {
  return (num < sd_get_num()) ? &sd_cards[num] : NULL;
}

size_t spi_get_num(void) { return count_of(spis); }

spi_t *spi_get_by_num(size_t num) {
  return (num < spi_get_num()) ? &spis[num] : NULL;
}