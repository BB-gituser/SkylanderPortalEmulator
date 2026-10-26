#include "../includes/utils.h"
#include "../includes/constant.h"
#include "../includes/sd.h"
#include "../includes/skylanders.h"
#include "../includes/usb.h"
#include "../includes/display.h"
#include "ff.h"
#include "pico/time.h"
#include <stddef.h>

int slot_index = 0;

uint32_t cur_time() {
  return to_ms_since_boot(get_absolute_time());
} // get the current time

Slot *get_slot_from_index(char index) {
  if (MAX_SKYLANDER_COUNT <= index)
    return NULL;
  return (slots_list + index);
}

void unload_slot(int index) {
  Slot *slot = get_slot_from_index(index);
  if (!slot || !(slot->fil))
    return;
  f_close(slot->fil);
  free(slot->fil);
  slot->fil = NULL;
  free(slot->skylander);
  slot->FileIndex = -1;
  slot->skylander = NULL;
  slot->statut = JUST_REMOVED;
  to_send += 2;
}

int load_slot(File *file, int *index) {
  *index = -1;
  int ori_index = slot_index == 0 ? 15 : slot_index - 1;
  while (((slots_list + slot_index)->statut != EMPTY) && (slot_index != ori_index)) {
    slot_index = (slot_index + 1) % MAX_SKYLANDER_COUNT;
  }
  if ((slots_list + slot_index)->statut != EMPTY) return -1;
  FIL fil;
  FRESULT fr = f_open(&fil, file->path, FA_READ | FA_WRITE);
  if (fr != FR_OK)
    return fr;
  SkylanderDump dump;
  fr = read_skylander_bin(&fil, &dump);
  if (fr != FR_OK) {
    f_close(&fil);
    return fr;
  }
  skylander_t *fig_data = get_skylander_data(dump.character_id, dump.variant_id);
  if (!fig_data) {
    f_close(&fil);
    return -2;
  }
  /*if (fig_data->id == 9999) {
    char text[64];
    snprintf(text, 64, "id='%d', v='%d'", dump.character_id, dump.variant_id);
    show_status("ERROR", text, 0xffff);
    while (1) {}
  }*/

  FIL *fil_ = malloc(sizeof(FIL));
  *fil_ = fil;

  skylander_t *fig_data_ = malloc(sizeof(skylander_t));
  *fig_data_ = *fig_data;

  Slot *s = (slots_list + slot_index);
  s->FileIndex = file->FileIndex;
  s->statut = JUST_ADDED;
  s->fil = fil_;
  s->skylander = fig_data_;
  *index = slot_index;
  slot_index = (slot_index + 1) % MAX_SKYLANDER_COUNT;
  to_send += 2;
  return 0;
}

void send_status() {
  uint8_t out[MSG_SIZE];
  create_status(out);
  send_report(out);
}

int get_nloaded_slots() {
  int len = MAX_SKYLANDER_COUNT;

  int out = 0;
  for (int i = 0; i < len; i++) {
    Slot *s = get_slot_from_index(i);
    if (s->statut == PRESENT)
      out++;
  }
  return out;
}

int get_loaded_slot_from_index_loaded(int index) {
  if (index < 0) return -1;
  int len = MAX_SKYLANDER_COUNT;

  for (int i = 0; i < len; i++) {
    Slot *s = get_slot_from_index(i);
    if (s->statut == PRESENT) {
      if (index == 0)
        return i;
      index--;
    }
  }
  return -1; // out
}

int File_is_used_by_index(int FileIndex) {
  int len = MAX_SKYLANDER_COUNT;

  for (int i = 0; i < len; i++) {
    Slot *s = get_slot_from_index(i);
    if (s->FileIndex == FileIndex) {
      return 1;
    }
  }
  return 0;
}

void create_status(uint8_t out[MSG_SIZE]) {
  for (int i = 0; i < MSG_SIZE; i++) out[i] = 0x00;
  out[0] = 0x53;
  uint32_t m = calculate_sens_bitmask();
  out[1] = m & 0xFF;
  out[2] = (m >> 8)  & 0xFF;
  out[3] = (m >> 16) & 0xFF;
  out[4] = (m >> 24) & 0xFF;
  out[5] = sense_counter++;
  out[6] = 0x01;
}

uint32_t calculate_sens_bitmask(void) {
  uint32_t mask = 0;
  for (int i = 0; i < MAX_SKYLANDER_COUNT; i++) {
    uint32_t bits;
    switch (slots_list[i].statut) {
      case PRESENT:
        bits = 0x1;
        break;
      case JUST_REMOVED:
        bits = 0x2;
        slots_list[i].statut = EMPTY;
        break;
      case JUST_ADDED:
        bits = 0x3;
        slots_list[i].statut = PRESENT;
        break;
      case EMPTY:
      default:
        bits = 0x0;
        break;
    }
    mask |= bits << (2 * i);
  }
  return mask;
}
