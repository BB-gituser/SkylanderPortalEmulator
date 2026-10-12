#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "ST7735_TFT.h"
#include "pico/stdlib.h"
#include "pico/time.h"

#include "../includes/constant.h"
#include "../includes/display.h"
#include "../includes/joystick.h"
#include "../includes/menu.h"
#include "../includes/pins.h"
#include "../includes/sd.h"
#include "../includes/utils.h"

/*
Main MENU
> player one --> Player MENU
> player two --> Player MENU
> explore --> Explore Menu

Player MENU
> back
> plot1 select file --> Explore Menu
> plot2 select file --> Explore Menu
> load
> unload

Explor Menu
> back
> * foldere / --> Explore Menu
> * file --> file menu

file MENU
> back
> select
> details

details MENU
> back
*/

uint32_t t0 = 0;
int edit = 1;

int current_selection = 0;
int menu_item_count = 0;
Folder *cur_folder = NULL;
int prev_menu = -1;

void init_menu() {
  fillScreen(ST7735_BLACK);
  current_selection = 0;
  menu_item_count = 0;
  edit = 1;
}

void render_menu() {
  if (prev_menu != menu_) {
    edit = 1;
    prev_menu = menu_;
  }
  if (t0 > to_ms_since_boot(get_absolute_time()))
    return;
  t0 = 0;
  if (menu_ == 0) {
    render_main_menu();
  } else if (menu_ == 1) {
    if (player_selected == 0) {
      menu_ = 0;
      return;
    } else if (player_selected == 1) {
      render_player_menu(1);
    } else if (player_selected == 2) {
      render_player_menu(2);
    }
  } else if (menu_ == 2) {
    if (player_selected == 0) {
      menu_ = 0;
      return;
    } else if (player_selected == 1) {
      select_file_render_menu(1, cur_folder);
    } else if (player_selected == 2) {
      select_file_render_menu(2, cur_folder);
    }
  } else {
    player_selected = 0;
    menu_ = 0;
  }
}

void render_main_menu() {
  const char *items[4] = {"PLAYER ONE", "PLAYER TWO", "OPTIONS", "EXPLORER"};
  JoystickStat s = joystick_get();
  if (s.top) {
    current_selection > 0 ? current_selection -= 1 : 0;
    edit = 1;
  } else if (s.bottom) {
    current_selection < 3 ? current_selection += 1 : 3;
    edit = 1;
  } else if (s.press) {
    if (current_selection == 0) {
      menu_ = 1;
      player_selected = 1;
    } else if (current_selection == 1) {
      menu_ = 1;
      player_selected = 2;
    } else {
      menu_ = 0;
      player_selected = 0;
    }
    init_menu();
    edit = 1;
    return;
  }
  if (!edit)
    return;
  drawText(4, 4, "MAIN MENU", ST7735_CYAN, ST7735_BLACK, 1);
  drawText(0, 14, "---------------------", ST7735_BLUE, ST7735_BLACK, 1);
  int start_index = 0;
  if (current_selection >= MAX_VISIBLE_ITEMS) {
    start_index = current_selection - MAX_VISIBLE_ITEMS + 1;
  }
  for (int i = 0; i < MAX_VISIBLE_ITEMS && (start_index + i) < 4; i++) {
    int idx = start_index + i;
    int y_pos = 26 + (i * 14);

    if (idx == current_selection) {
      drawText(4, y_pos, ">", ST7735_YELLOW, ST7735_BLACK, 1);
      drawText(14, y_pos, items[idx], ST7735_YELLOW, ST7735_BLACK, 1);
    } else {
      drawText(4, y_pos, ">", ST7735_BLACK, ST7735_BLACK, 1);
      drawText(14, y_pos, items[idx], ST7735_WHITE, ST7735_BLACK, 1);
    }
  }
  edit = 0;
}

void render_player_menu(int player) {
  const char *items[4] = {"PLOT 1", "PLOT 2 if swap force", "LOAD", "UNLOAD"};

  JoystickStat s = joystick_get();
  if (s.top) {
    current_selection > 0 ? current_selection -= 1 : 0;
    edit = 1;
  }
  if (s.bottom) {
    current_selection < 3 ? current_selection += 1 : 3;
    edit = 1;
  }
  if (s.press) {
    edit = 1;
    if (current_selection == 0) {
      menu_ = 2;
      cur_folder = root;
      edit = 1;
    } else if (current_selection == 3) {
      *(loaded_skylanders + player - 1) = NULL;
      just_remove = 1;
    } else {
      menu_ = 0;
      player_selected = 0;
    };
    init_menu();
    return;
  }
  if (s.left) {
    edit = 1;
    menu_ = 0;
    player_selected = 0;
    init_menu();
    return;
  }
  if (!edit)
    return;
  char msg[COLS + 1];
  snprintf(msg, sizeof msg, "PLAYER %d | load %d", player,
           *(loaded_skylanders + player - 1) != NULL);
  drawText(4, 4, msg, ST7735_CYAN, ST7735_BLACK, 1);
  drawText(0, 14, "---------------------", ST7735_BLUE, ST7735_BLACK, 1);
  drawText(2, 24, "press left to back", ST7735_BLUE, ST7735_BLACK, 1);
  int start_index = 0;
  if (current_selection >= MAX_VISIBLE_ITEMS) {
    start_index = current_selection - MAX_VISIBLE_ITEMS + 1;
  }
  for (int i = 0; i < MAX_VISIBLE_ITEMS && (start_index + i) < 4; i++) {
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
  edit = 0;
}

void select_file_render_menu(int player, Folder *folder) {
  if (!folder)
    return;
  int max_select = folder->folder_count + folder->file_count;
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
  // const char *items[4] = {"PLOT 1", "PLOT 2 if swap force", "LOAD",
  // "UNLOAD"};

  JoystickStat s = joystick_get();
  if (s.top) {
    edit = 1;
    current_selection > 0 ? current_selection -= 1 : 0;
  }
  if (s.bottom) {
    edit = 1;
    current_selection < max_select - 1 ? current_selection += 1
                                       : max_select - 1;
  }
  if (s.press) {
    edit = 1;
    if (current_selection < folder->folder_count) {
      cur_folder = ((folder->folderlist) + current_selection);
      edit = 1;
    } else {
      File F = *((folder->filelist) + current_selection - folder->folder_count);
      if (sd_file_open(&F, FA_READ | FA_WRITE) != FR_OK) {
        show_status("ERROR LOAD", "*(loaded_skylanders + player) stay NULL",
                    ST7735_RED);
        init_menu();
        edit = 1;
        return;
      }
      
      if (fd_in_array(F.item, loaded_skylanders, MAX_SKYLANDER_COUNT) != 0)
        return;
      
        if (*(loaded_skylanders + player - 1)) {
        // unload
        FIL *toClose = *(loaded_skylanders + player - 1);
        f_close(toClose);
        free(toClose);
        *(loaded_skylanders + player - 1) = NULL;

        char *nbuffer = calloc(MSG_SIZE, 1);
        nbuffer[0] = 0x53;
        nbuffer[1] =
            create_sense_bitmask(loaded_skylanders, MAX_SKYLANDER_COUNT);
        nbuffer[5] = sense_counter++;
        nbuffer[6] = 0x01;
        tud_hid_report(0, nbuffer, MSG_SIZE);
        sleep_ms(500);
        free(nbuffer);
      }
      
      *(loaded_skylanders + player - 1) = F.item;
      show_status("SELECT", (char *)(F.filename), ST7735_WHITE);
      t0 = to_ms_since_boot(get_absolute_time()) + 3000;
      if (!(*(loaded_skylanders + player - 1))) {
        show_status("ERROR LOAD", "*(loaded_skylanders + player) stay NULL",
                    ST7735_RED);
        t0 = to_ms_since_boot(get_absolute_time()) + 3000;
      }
      just_add = 1;
      init_menu();
      menu_ = 1;
      edit = 1;
      return;
    }
    init_menu();
    return;
  }
  if (s.left) {
    edit = 1;
    if (!(folder->parent)) {
      menu_ = 1;
    } else {
      cur_folder = folder->parent;
      edit = 1;
    }
    init_menu();
    return;
  }
  if (!edit)
    return;
  char msg[COLS + 1];
  snprintf(msg, sizeof msg, "SELECT FILE %d", player);
  drawText(4, 4, msg, ST7735_CYAN, ST7735_BLACK, 1);
  drawText(0, 14, "---------------------", ST7735_BLUE, ST7735_BLACK, 1);
  snprintf(msg, sizeof msg, "press left to back, %d",
           folder->folder_count + folder->file_count);
  drawText(2, 24, msg, ST7735_BLUE, ST7735_BLACK, 1);
  int start_index = 0;
  if (current_selection >= MAX_VISIBLE_ITEMS) {
    start_index = current_selection - MAX_VISIBLE_ITEMS + 1;
  }
  for (int i = 0; i < MAX_VISIBLE_ITEMS && (start_index + i) < max_select;
       i++) {
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
  edit = 0;
}