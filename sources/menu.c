#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "ST7735_TFT.h"
#include "pico/stdlib.h"
#include "pico/time.h"

#include "../includes/constant.h"
#include "../includes/display.h"
#include "../includes/documentation.h"
#include "../includes/joystick.h"
#include "../includes/menu.h"
#include "../includes/pins.h"
#include "../includes/sd.h"
#include "../includes/usb.h"
#include "../includes/utils.h"

uint32_t can_refresh =
    0; // stop instructions in the loop while current time < can_refresh
int refresh_menu = 1;      // allow to refresh menu calling draw_menu
int reinit_menu = 0;       // allow to call init_menu
int current_selection = 0; // current index of selection in the menu
int current_menu = 0;      // current menu
Folder *cur_folder = NULL; // current folder when explore files
int fig_index = -1;        // for figurines menu

int scroll_index = 0;

void wait(uint32_t t) { // set can_refresh to current time + t (ms)
  can_refresh = cur_time() + t;
}

void draw_slide() {
  if (current_selection == 0) {
    fillRoundRect(10, 35, SCREEN_W - 20, SCREEN_H - 45, 15,
                  rgb888_to_rgb565(127, 127, 127));
    drawRoundRect(10, 35, SCREEN_W - 20, SCREEN_H - 45, 15,
                  rgb888_to_rgb565(0, 255, 0));
    // fillRoundRect(SCREEN_W / 2 - 10, (SCREEN_H - 35) / 2 + 35, SCREEN_W / 2 +
    // 10, 10, 5, rgb888_to_rgb565(0, 255, 0));
    int centre_x = 10 + (SCREEN_W - 20) / 2;
    int centre_y = 35 + (SCREEN_H - 45) / 2;
    fillRoundRect(centre_x - 40, centre_y - 10, 80, 20, 10,
                  rgb888_to_rgb565(0, 255, 0));
    fillRoundRect(centre_x - 10, centre_y - 40, 20, 80, 10,
                  rgb888_to_rgb565(0, 255, 0));
  } else if (current_selection == 1) { // doc
    fillRoundRect(10, 35, SCREEN_W - 20, SCREEN_H - 45, 15,
                  rgb888_to_rgb565(127, 127, 127));
    drawRoundRect(10, 35, SCREEN_W - 20, SCREEN_H - 45, 15,
                  rgb888_to_rgb565(0, 0, 0));
    drawText(15, 40, "not", rgb888_to_rgb565(0, 0, 0),
             rgb888_to_rgb565(127, 127, 127), 2);
    drawText(15, 80, "implemented", rgb888_to_rgb565(0, 0, 0),
             rgb888_to_rgb565(127, 127, 127), 2);
  } else {
    int nloaded_slots = get_nloaded_slots();
    int slot_i = get_loaded_slot_from_index_loaded(current_selection - 2);
    if (slot_i == -1)
      return;
    Slot *slot = get_slot_from_index(slot_i);
    if (!slot)
      return;
    skylander_t *fig = slot->skylander;

    const char *filename = NULL;
    switch (fig->element) {
    case SKY_ELEM_NONE:
      fillRoundRect(10, 35, SCREEN_W - 20, SCREEN_H - 45, 15,
                    rgb888_to_rgb565(128, 128, 128));
      drawRoundRect(10, 35, SCREEN_W - 20, SCREEN_H - 45, 15,
                    rgb888_to_rgb565(64, 64, 64));
      filename = "none";
      break;

    case SKY_ELEM_AIR:
      fillRoundRect(10, 35, SCREEN_W - 20, SCREEN_H - 45, 15,
                    rgb888_to_rgb565(135, 206, 250));
      drawRoundRect(10, 35, SCREEN_W - 20, SCREEN_H - 45, 15,
                    rgb888_to_rgb565(70, 130, 180));
      filename = "air";
      break;

    case SKY_ELEM_EARTH:
      fillRoundRect(10, 35, SCREEN_W - 20, SCREEN_H - 45, 15,
                    rgb888_to_rgb565(139, 69, 19));
      drawRoundRect(10, 35, SCREEN_W - 20, SCREEN_H - 45, 15,
                    rgb888_to_rgb565(92, 51, 23));
      filename = "earth";
      break;

    case SKY_ELEM_FIRE:
      fillRoundRect(10, 35, SCREEN_W - 20, SCREEN_H - 45, 15,
                    rgb888_to_rgb565(255, 69, 0));
      drawRoundRect(10, 35, SCREEN_W - 20, SCREEN_H - 45, 15,
                    rgb888_to_rgb565(178, 34, 34));
      filename = "fire";
      break;

    case SKY_ELEM_WATER:
      fillRoundRect(10, 35, SCREEN_W - 20, SCREEN_H - 45, 15,
                    rgb888_to_rgb565(30, 144, 255));
      drawRoundRect(10, 35, SCREEN_W - 20, SCREEN_H - 45, 15,
                    rgb888_to_rgb565(0, 0, 128));
      filename = "water";
      break;

    case SKY_ELEM_MAGIC:
      fillRoundRect(10, 35, SCREEN_W - 20, SCREEN_H - 45, 15,
                    rgb888_to_rgb565(138, 43, 226));
      drawRoundRect(10, 35, SCREEN_W - 20, SCREEN_H - 45, 15,
                    rgb888_to_rgb565(75, 0, 130));
      filename = "magic";
      break;

    case SKY_ELEM_TECH:
      fillRoundRect(10, 35, SCREEN_W - 20, SCREEN_H - 45, 15,
                    rgb888_to_rgb565(192, 192, 192));
      drawRoundRect(10, 35, SCREEN_W - 20, SCREEN_H - 45, 15,
                    rgb888_to_rgb565(105, 105, 105));
      filename = "tech";
      break;

    case SKY_ELEM_LIFE:
      fillRoundRect(10, 35, SCREEN_W - 20, SCREEN_H - 45, 15,
                    rgb888_to_rgb565(50, 205, 50));
      drawRoundRect(10, 35, SCREEN_W - 20, SCREEN_H - 45, 15,
                    rgb888_to_rgb565(34, 139, 34));
      filename = "life";
      break;

    case SKY_ELEM_UNDEAD:
      fillRoundRect(10, 35, SCREEN_W - 20, SCREEN_H - 45, 15,
                    rgb888_to_rgb565(47, 79, 79));
      drawRoundRect(10, 35, SCREEN_W - 20, SCREEN_H - 45, 15,
                    rgb888_to_rgb565(0, 0, 0));
      filename = "undead";
      break;

    case SKY_ELEM_LIGHT:
      fillRoundRect(10, 35, SCREEN_W - 20, SCREEN_H - 45, 15,
                    rgb888_to_rgb565(255, 255, 0));
      drawRoundRect(10, 35, SCREEN_W - 20, SCREEN_H - 45, 15,
                    rgb888_to_rgb565(218, 165, 32));
      filename = "light";
      break;

    case SKY_ELEM_DARK:
      fillRoundRect(10, 35, SCREEN_W - 20, SCREEN_H - 45, 15,
                    rgb888_to_rgb565(40, 40, 40));
      drawRoundRect(10, 35, SCREEN_W - 20, SCREEN_H - 45, 15,
                    rgb888_to_rgb565(10, 10, 10));
      filename = "dark";
      break;

    case SKY_ELEM_KAOS:
      fillRoundRect(10, 35, SCREEN_W - 20, SCREEN_H - 45, 15,
                    rgb888_to_rgb565(139, 0, 139));
      drawRoundRect(10, 35, SCREEN_W - 20, SCREEN_H - 45, 15,
                    rgb888_to_rgb565(75, 0, 130));
      filename = "kaos";
      break;

    case SKY_ELEM_UNKNOWN:
    case SKY_ELEM_COUNT:
    default:
      fillRoundRect(10, 35, SCREEN_W - 20, SCREEN_H - 45, 15,
                    rgb888_to_rgb565(50, 50, 50));
      drawRoundRect(10, 35, SCREEN_W - 20, SCREEN_H - 45, 15,
                    rgb888_to_rgb565(100, 100, 100));
      filename = NULL;
      break;
    }
    if (filename != NULL) {
      char filepath[64];
      snprintf(filepath, sizeof(filepath), "0:/pictures/%s.bmp", filename);
      bmpDraw(15, 40, (uint8_t *)filepath);
    }
    setTextWrap(true);
    drawText(15, 80, fig->name, rgb888_to_rgb565(0, 0, 0),
             rgb888_to_rgb565(255, 255, 255), 2);

    setTextWrap(false);
  }
}

void init_menu() {
  reset_screen(ST7735_BLACK);
  reinit_menu = 0;
  current_selection = 0;
  refresh_menu = 1;
}

void draw_menu(const char *title, char **items, int nb_items) {
  fillRoundRect(0, 0, SCREEN_W, 20, 15, rgb888_to_rgb565(150, 150, 150));
  fillRectangle(0, 0, SCREEN_W, 10, rgb888_to_rgb565(150, 150, 150));
  drawText(10, 5, title, rgb888_to_rgb565(255, 0, 0),
           rgb888_to_rgb565(150, 150, 150), 2);
  drawText(2, 24, "press left to back", ST7735_BLUE, ST7735_BLACK, 1);
  int start_index = 0;
  if (current_selection >= MAX_VISIBLE_ITEMS) {
    start_index = current_selection - MAX_VISIBLE_ITEMS + 1;
  }
  for (int i = 0; i < MAX_VISIBLE_ITEMS && (start_index + i) < nb_items; i++) {
    int idx = start_index + i;
    int y_pos = 36 + (i * 14);

    if (idx == current_selection) {
      drawText(4, y_pos, ">", ST7735_YELLOW, ST7735_BLACK, 1);
      drawText(14, y_pos, items[idx], ST7735_YELLOW, ST7735_BLACK, 1);
    } else {
      drawText(4, y_pos, ">", ST7735_BLACK, ST7735_BLACK, 1);
      drawText(14, y_pos, items[idx], ST7735_WHITE, ST7735_BLACK, 1);
    }
  }
}

void render_figurine_menu() {
  if (fig_index == -1) {
    current_menu = 0;
    refresh_menu = 1;
    reinit_menu = 1;
    show_status("ERROR", "No figs", 0xff00);
    wait(1000);
    return;
  }

  JoystickStat s = joystick_get();
  if (s.top) {
    current_selection = (current_selection == 0) ? 0 : current_selection - 1;
    if (current_selection < 0)
      current_selection = 0;
    refresh_menu = 1;
  } else if (s.bottom) {
    current_selection = (current_selection == 2) ? 2 : current_selection + 1;
    refresh_menu = 1;
  }
  if (s.press) {
    reinit_menu = 1;
    if (current_selection == 0) {
      reinit_menu = 1;
      current_menu = 0;
      refresh_menu = 1;
      return;
    }
    if (current_selection == 1) {
      int slot_i = get_loaded_slot_from_index_loaded(fig_index);
      if (slot_i == -1) {
        show_status("ERRORS", "fail on unload figurine", 0xffff);
        wait(1000);
        return;
      }
      unload_slot(slot_i);
      show_status("UNLOAD", "UNLOAD SUCCESS", 0xffff);
      wait(1000);
      reinit_menu = 1;
      current_menu = 0;
      refresh_menu = 1;
      current_selection = 0;
      return;
    }
  }

  if (!refresh_menu)
    return;

  int nloaded_slots = get_nloaded_slots();
  int slot_i = get_loaded_slot_from_index_loaded(fig_index);
  if (slot_i == -1) {
    show_status("NO figures", "bad index", 0xffff);
    wait(1000);
  }
  Slot *slot = get_slot_from_index(slot_i);
  if (!slot) {
    show_status("NO SLOT", "no slot found", 0xffff);
    wait(1000);
  }
  skylander_t *fig = slot->skylander;

  char title[64] = {0};
  snprintf(title, 64, "menu %s", fig->name);
  char *items[] = {"back to menu", "unload", "infos"};

  draw_menu(title, items, 3);
  refresh_menu = 0;
}

void render_main_menu() {
  fig_index = -1;
  JoystickStat s = joystick_get();
  if (s.left) {
    current_selection = current_selection == 0 ? 0 : current_selection - 1;
    refresh_menu = 1;
  } else if (s.right) {
    current_selection = current_selection == 1 + get_nloaded_slots()
                            ? 1 + get_nloaded_slots()
                            : current_selection + 1;
    refresh_menu = 1;
  }
  if (s.press) {
    reinit_menu = 1;
    if (current_selection == 0) {
      reinit_menu = 1;
      current_menu = 2;
      return;
    }
    if (current_selection == 1) {
      scroll_index = 0;
      current_menu = 1;
      return;
    } else {
      fig_index = current_selection - 2;
      current_menu = 3;
      reinit_menu = 1;
      current_selection = 0;
      refresh_menu = 1;
      return;
    }
  }

  // draw
  if (!refresh_menu)
    return;
  fillRoundRect(0, 0, SCREEN_W, 30, 15, rgb888_to_rgb565(150, 150, 150));
  fillRectangle(0, 0, SCREEN_W, 10, rgb888_to_rgb565(150, 150, 150));
  drawText(0, 10, "SKYPORTALEMUL", rgb888_to_rgb565(255, 255, 0),
           rgb888_to_rgb565(150, 150, 150), 2);
  refresh_menu = 0;

  draw_slide();
}

void select_file_render_menu() {
  if (!cur_folder)
    cur_folder = root;
  Folder *folder = cur_folder;
  int max_select = folder->folder_count + folder->file_count;

  JoystickStat s = joystick_get();
  if (s.top) {
    refresh_menu = 1;
    current_selection > 0 ? current_selection -= 1 : 0;
  }
  if (s.bottom) {
    refresh_menu = 1;
    current_selection < max_select - 1 ? current_selection += 1
                                       : max_select - 1;
  }
  if (s.left) {
    refresh_menu = 1;
    if (!(folder->parent)) {
      current_menu = 0;
      reinit_menu = 1;
    } else {
      cur_folder = folder->parent;
      current_selection = 0;
    }
    init_menu();
    return;
  }
  if (s.press) {
    if (current_selection < (folder->folder_count)) {
      cur_folder = ((folder->folderlist) + current_selection);
      refresh_menu = 1;
      current_selection = 0;
      reinit_menu = 1;
      return;
    } else {
      File *F =
          ((folder->filelist) + current_selection - (folder->folder_count));
      if (File_is_used_by_index(F->FileIndex)) {
        show_status("ERROR", "file already used", ST7735_RED);
        wait(2000);
        return;
      }
      int index_;
      int res = load_slot(F, &index_);
      if (res) {
        char text[128] = {0};
        snprintf(text, 128, "an error occured : %d\npaths : `%s`", res,
                 F->path);
        show_status("ERROR", text, ST7735_RED);
        wait(2000);
        return;
      }
      refresh_menu = 1;
      reinit_menu = 1;
      current_menu = 0;
      show_status("SUCCESS", "Skylanders is loaded", ST7735_GREEN);
      wait(2000);
      cur_folder = NULL;
      return;
    }
  }
  if (!refresh_menu)
    return;
  char *items[folder->folder_count + folder->file_count];
  int pos = 0;
  for (int i = 0; i < folder->folder_count; i++) {
    *(items + pos) = ((folder->folderlist) + i)->foldername;
    pos++;
  }
  for (int i = 0; i < folder->file_count; i++) {
    *(items + pos) = ((folder->filelist) + i)->filename;
    pos++;
  }
  reset_screen(ST7735_BLACK);

  draw_menu("SELECT FILE", items, max_select);
  refresh_menu = 0;
}

void doc_menu() {
  JoystickStat s = joystick_get();
  if (s.top) {
    refresh_menu = 1;
    scroll_index--;
  }
  if (s.bottom) {
    refresh_menu = 1;
    scroll_index++;
  }
  if (s.left) {
    refresh_menu = 1;
    current_menu = 0;
    reinit_menu = 1;
    current_selection = 0;
    init_menu();
    NormalDisplay();
    setRotation(1);
    return;
  }
  if (!refresh_menu)
    return;
  setTextWrap(true);
  char *doc_text = "DOCUMENTATION"
                   "-------------"
                   "Skylanders portal emulator"
                   "It functions like a portal"
                   "you can add and remove figurines (bin files)"
                   "if you have any bugs, please reports them";

  setScrollDefinition(10, 10, 0);
  setRotation(1);
  fillRectangle(0, 0, SCREEN_W, SCREEN_H, ST7735_BLACK);
  fillRectangle(0, 10, SCREEN_W, SCREEN_H - 10,
                rgb888_to_rgb565(200, 200, 200));
  drawText(5, 15, doc_text, ST7735_BLACK, rgb888_to_rgb565(200, 200, 200), 1);
  VerticalScroll(scroll_index);

  setTextWrap(false);
  refresh_menu = 0;
}

void menu_screen_refresh() {
  if (cur_time() > can_refresh) {
    if (reinit_menu) {
      reset_screen(ST7735_BLACK);
      reinit_menu = 0;
      current_selection = 0;
      refresh_menu = 1;
    }
    can_refresh = cur_time() + 1000 / 24;

    // action
    if (current_menu == 0)
      render_main_menu();
    if (current_menu == 1) {
      current_menu = 0;
    } //{doc_menu();} // infos
    if (current_menu == 2) {
      select_file_render_menu();
    }
    if (current_menu == 3) {
      render_figurine_menu();
    }
  }
}
