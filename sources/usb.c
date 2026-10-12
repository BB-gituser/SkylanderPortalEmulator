#include <stdio.h>
#include <stdlib.h>

#include "tusb.h"

#include "../includes/constant.h"
#include "../includes/display.h"
#include "../includes/usb.h"
#include "../includes/utils.h"

// doc https://docs.tinyusb.org/en/latest/reference/class/device.html

static FIL *slot_file(uint8_t index_byte) {
  uint8_t slot = index_byte & 0x0F;
  if (slot >= MAX_SKYLANDER_COUNT) {
    return NULL;
  }
  return loaded_skylanders[slot];
}

static void send_report(const uint8_t *report) {
  if (!tud_hid_report(0, report, MSG_SIZE)) {
    return;
  }
}

void tud_mount_cb(void) {}

// Invoked when device is unmounted
void tud_umount_cb(void) {}

void tud_suspend_cb(bool remote_wakeup_en) { (void)remote_wakeup_en; }

void tud_resume_cb(void) {}

void tud_hid_report_complete_cb(uint8_t instance, uint8_t const *report,
                                uint8_t len) {
  (void)instance; // usb émmulé (toujours le meme dans notre cas)
  (void)report;
  (void)len; // taille du report
}

uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id,
                               hid_report_type_t report_type, uint8_t *buffer,
                               uint16_t reqlen) {
  (void)instance;    // usb émmulé (toujours le meme dans notre cas)
  (void)report_id;   // ID du repport demandé
  (void)report_type; // type de repport demandé
  (void)buffer;      // output
  (void)reqlen;      // taille output

  return 0;
}



void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id,
                           hid_report_type_t report_type, uint8_t const *buffer,
                           uint16_t bufsize) {
  (void)instance;    // usb émmulé (toujours le meme dans notre cas)
  (void)report_id;   // ID du repport demandé
  (void)report_type; // type de repport demandé
  (void)buffer;      // input
  (void)bufsize;     // taille input
  
  uint8_t out[MSG_SIZE];
  if (report_type != HID_REPORT_TYPE_OUTPUT &&
      report_type != HID_REPORT_TYPE_INVALID) {
    return;
  }

  if (bufsize < 1) {
    return;
  }

  char b = *buffer;

   
  memset(out, 0, sizeof out);

  if (b == 'R') {
    shutthefuckup = true;
    out[0] = 0x52;
    out[1] = 0x02;
    out[2] = 0x1b;
    send_report(out);
  } else if (b == 'J' || b == 'M') { // son ou led pas important
    out[0] = b;
    send_report(out);
  } else if (b == 'A') { // active le portail
    out[0] = 0x41;
    out[1] = (bufsize > 1) ? buffer[1] : 0;
    out[2] = 0xff;
    out[3] = 0x77;
    send_report(out);
    if (bufsize > 1 && buffer[1] == 0x01) {
      shutthefuckup = false;
    }
  } else if (b == 'S') { // etats des figurines
    out[0] = 0x53;
    out[1] = create_sense_bitmask(loaded_skylanders, MAX_SKYLANDER_COUNT);
    out[5] = sense_counter++;
    out[6] = 0x01;
    
    send_report(out);
  } else if (b == 'Q') { // lire block de 16 octets
    if (bufsize < 3) {
      return;
    }
    //char msg[COLS + 1];
    //snprintf(msg, sizeof msg, "detect %X %c %s %16X", b, b, buffer, buffer);
    //drawText(2, 4, msg, ST7735_GREEN, ST7735_BLACK, 1);
    FIL *f = slot_file(buffer[1]);
    if (!f) return;
    out[0] = 0x51;
    out[2] = buffer[2];

    out[1] = 0x10 | (buffer[1] & 0x0F);
    /*
    *out = 0x51;
    *(out + 1) = 0x10;

    if (buffer[1] == 0x10 || buffer[1] == 0x20 || buffer[1] == 0x30 ||
        buffer[1] == 0x40 || buffer[1] == 0x50 || buffer[1] == 0x60 ||
        buffer[1] == 0x70 || buffer[1] == 0x80 || buffer[1] == 0x90 ||
        buffer[1] == 0xA0 || buffer[1] == 0xB0 || buffer[1] == 0xC0 ||
        buffer[1] == 0xD0 || buffer[1] == 0xE0 || buffer[1] == 0xF0) {
      *(out + 1) = 0x10;
    }
    if (buffer[1] == 0x11 || buffer[1] == 0x21 || buffer[1] == 0x31 ||
        buffer[1] == 0x41 || buffer[1] == 0x51 || buffer[1] == 0x61 ||
        buffer[1] == 0x71 || buffer[1] == 0x81 || buffer[1] == 0x91 ||
        buffer[1] == 0xA1 || buffer[1] == 0xB1 || buffer[1] == 0xC1 ||
        buffer[1] == 0xD1 || buffer[1] == 0xE1 || buffer[1] == 0xF1) {
      *(out + 1) = 0x11;
    }
    if (buffer[1] == 0x12 || buffer[1] == 0x22 || buffer[1] == 0x32 ||
        buffer[1] == 0x42 || buffer[1] == 0x52 || buffer[1] == 0x62 ||
        buffer[1] == 0x72 || buffer[1] == 0x82 || buffer[1] == 0x92 ||
        buffer[1] == 0xA2 || buffer[1] == 0xB2 || buffer[1] == 0xC2 ||
        buffer[1] == 0xD2 || buffer[1] == 0xE2 || buffer[1] == 0xF2) {
      *(out + 1) = 0x12;
    }

    *(out + 2) = buffer[2];
    midbuffer = calloc(BLOCK_SIZE, 1);

    if (buffer[1] == 0x10 || buffer[1] == 0x20 || buffer[1] == 0x30 ||
        buffer[1] == 0x40 || buffer[1] == 0x50 || buffer[1] == 0x60 ||
        buffer[1] == 0x70 || buffer[1] == 0x80 || buffer[1] == 0x90 ||
        buffer[1] == 0xA0 || buffer[1] == 0xB0 || buffer[1] == 0xC0 ||
        buffer[1] == 0xD0 || buffer[1] == 0xE0 || buffer[1] == 0xF0) {
      curfile = loaded_skylanders[0];
    }
    if (buffer[1] == 0x11 || buffer[1] == 0x21 || buffer[1] == 0x31 ||
        buffer[1] == 0x41 || buffer[1] == 0x51 || buffer[1] == 0x61 ||
        buffer[1] == 0x71 || buffer[1] == 0x81 || buffer[1] == 0x91 ||
        buffer[1] == 0xA1 || buffer[1] == 0xB1 || buffer[1] == 0xC1 ||
        buffer[1] == 0xD1 || buffer[1] == 0xE1 || buffer[1] == 0xF1) {
      curfile = loaded_skylanders[1];
    }
    if (buffer[1] == 0x12 || buffer[1] == 0x22 || buffer[1] == 0x32 ||
        buffer[1] == 0x42 || buffer[1] == 0x52 || buffer[1] == 0x62 ||
        buffer[1] == 0x72 || buffer[1] == 0x82 || buffer[1] == 0x92 ||
        buffer[1] == 0xA2 || buffer[1] == 0xB2 || buffer[1] == 0xC2 ||
        buffer[1] == 0xD2 || buffer[1] == 0xE2 || buffer[1] == 0xF2) {
      curfile = loaded_skylanders[2];
    }*/

    UINT actual_len = 0;
    f_lseek(f, buffer[2] * BLOCK_SIZE);
    FRESULT fr = f_read(f, out + 3, BLOCK_SIZE, &actual_len);

    if (fr != FR_OK || actual_len != BLOCK_SIZE) {
      printf("Read: fr=%d len=%u (attendu %u)\n", (int)fr, (unsigned)actual_len,
             (unsigned)BLOCK_SIZE);
      return;
    }
    send_report(out);
  } else if (b == 'W') {
    if (bufsize < 3 + BLOCK_SIZE) {
      return;
    }
    FIL *f = slot_file(buffer[1]);
    if (f == NULL) {
      return;
    }

    UINT actual_len = 0;
    f_lseek(f, buffer[2] * BLOCK_SIZE); // positionner AVANT d'ecrire
    FRESULT fr = f_write(f, buffer + 3, BLOCK_SIZE, &actual_len);
    f_sync(f); // sinon la modif reste en cache et n'arrive pas sur la SD
    if (fr != FR_OK || actual_len != BLOCK_SIZE) {
      printf("Write: fr=%d len=%u (attendu %u)\n", (int)fr,
             (unsigned)actual_len, (unsigned)BLOCK_SIZE);
      return;
    }

    // reponse = echo de la requete (comme avant), avec l'index corrige
    memcpy(out, buffer, 3 + BLOCK_SIZE);
    out[1] = 0x10 | (buffer[1] & 0x0F);
    send_report(out);
    /*
    uint actual_len = 0;
    FIL *curfile;
    char *midbuffer;
    midbuffer = calloc(BLOCK_SIZE, 1);
    if (buffer[1] == 0x10 || buffer[1] == 0x20 || buffer[1] == 0x30 ||
        buffer[1] == 0x40 || buffer[1] == 0x50 || buffer[1] == 0x60 ||
        buffer[1] == 0x70 || buffer[1] == 0x80 || buffer[1] == 0x90 ||
        buffer[1] == 0xA0 || buffer[1] == 0xB0 || buffer[1] == 0xC0 ||
        buffer[1] == 0xD0 || buffer[1] == 0xE0 || buffer[1] == 0xF0) {
      curfile = loaded_skylanders[0];
    }
    if (buffer[1] == 0x11 || buffer[1] == 0x21 || buffer[1] == 0x31 ||
        buffer[1] == 0x41 || buffer[1] == 0x51 || buffer[1] == 0x61 ||
        buffer[1] == 0x71 || buffer[1] == 0x81 || buffer[1] == 0x91 ||
        buffer[1] == 0xA1 || buffer[1] == 0xB1 || buffer[1] == 0xC1 ||
        buffer[1] == 0xD1 || buffer[1] == 0xE1 || buffer[1] == 0xF1) {
      curfile = loaded_skylanders[1];
    }
    if (buffer[1] == 0x12 || buffer[1] == 0x22 || buffer[1] == 0x32 ||
        buffer[1] == 0x42 || buffer[1] == 0x52 || buffer[1] == 0x62 ||
        buffer[1] == 0x72 || buffer[1] == 0x82 || buffer[1] == 0x92 ||
        buffer[1] == 0xA2 || buffer[1] == 0xB2 || buffer[1] == 0xC2 ||
        buffer[1] == 0xD2 || buffer[1] == 0xE2 || buffer[1] == 0xF2) {
      curfile = loaded_skylanders[2];
    }

    midbuffer[0] = buffer[3];
    midbuffer[1] = buffer[4];
    midbuffer[2] = buffer[5];
    midbuffer[3] = buffer[6];
    midbuffer[4] = buffer[7];
    midbuffer[5] = buffer[8];
    midbuffer[6] = buffer[9];
    midbuffer[7] = buffer[10];
    midbuffer[8] = buffer[11];
    midbuffer[9] = buffer[12];
    midbuffer[10] = buffer[13];
    midbuffer[11] = buffer[14];
    midbuffer[12] = buffer[15];
    midbuffer[13] = buffer[16];
    midbuffer[14] = buffer[17];
    midbuffer[15] = buffer[18];

    f_write(curfile, midbuffer, BLOCK_SIZE, &actual_len);

    if (actual_len != BLOCK_SIZE) {
      // error writing
      printf("Read data length is %i not %i", actual_len, BLOCK_SIZE);
      return;
    }

    free(midbuffer);

    out = calloc(MSG_SIZE, 1);

    out[0] = buffer[0];
    out[1] = buffer[1];
    out[2] = buffer[2];
    out[3] = buffer[3];
    out[4] = buffer[4];
    out[5] = buffer[5];
    out[6] = buffer[6];
    out[7] = buffer[7];
    out[8] = buffer[8];
    out[9] = buffer[9];
    out[10] = buffer[10];
    out[11] = buffer[11];
    out[12] = buffer[12];
    out[13] = buffer[13];
    out[14] = buffer[14];
    out[15] = buffer[15];
    out[16] = buffer[16];
    out[17] = buffer[17];
    out[18] = buffer[18];

    if (buffer[1] == 0x10 || buffer[1] == 0x20 || buffer[1] == 0x30 ||
        buffer[1] == 0x40 || buffer[1] == 0x50 || buffer[1] == 0x60 ||
        buffer[1] == 0x70 || buffer[1] == 0x80 || buffer[1] == 0x90 ||
        buffer[1] == 0xA0 || buffer[1] == 0xB0 || buffer[1] == 0xC0 ||
        buffer[1] == 0xD0 || buffer[1] == 0xE0 || buffer[1] == 0xF0) {
      out[1] = 0x10;
    }
    if (buffer[1] == 0x11 || buffer[1] == 0x21 || buffer[1] == 0x31 ||
        buffer[1] == 0x41 || buffer[1] == 0x51 || buffer[1] == 0x61 ||
        buffer[1] == 0x71 || buffer[1] == 0x81 || buffer[1] == 0x91 ||
        buffer[1] == 0xA1 || buffer[1] == 0xB1 || buffer[1] == 0xC1 ||
        buffer[1] == 0xD1 || buffer[1] == 0xE1 || buffer[1] == 0xF1) {
      out[1] = 0x11;
    }
    if (buffer[1] == 0x12 || buffer[1] == 0x22 || buffer[1] == 0x32 ||
        buffer[1] == 0x42 || buffer[1] == 0x52 || buffer[1] == 0x62 ||
        buffer[1] == 0x72 || buffer[1] == 0x82 || buffer[1] == 0x92 ||
        buffer[1] == 0xA2 || buffer[1] == 0xB2 || buffer[1] == 0xC2 ||
        buffer[1] == 0xD2 || buffer[1] == 0xE2 || buffer[1] == 0xF2) {
      out[1] = 0x12;
    }
    tud_hid_report(0, out, bufsize);
    free(out);*/
  } else if (b == 'c') {
    uint16_t n = bufsize < MSG_SIZE ? bufsize : MSG_SIZE;
    memcpy(out, buffer, n);
    send_report(out);
  } 

  return;
}

// pour envoyer des données
// bool tud_hid_report(uint8_t report_id, void const* report, uint8_t len);
// bool tud_hid_report(0, void const* report, uint8_t len);

void tud_task_() { tud_task(); }