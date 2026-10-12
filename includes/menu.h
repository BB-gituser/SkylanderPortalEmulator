#ifndef MENU_H
#define MENU_H

#define MAX_VISIBLE_ITEMS 6

void init_menu();

void edit_menu();

void render_menu();

void render_main_menu();

void render_player_menu(int player);

void select_file_render_menu(int player, Folder *folder);

#endif