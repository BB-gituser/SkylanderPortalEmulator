#ifndef CST_H
#define CST_H

#include <stdint.h>
#include <stdbool.h>

#include "ff.h"

#include "../includes/skylanders.h"
#include "../includes/sd.h"


// define
#define MSG_SIZE 32
#define BLOCK_SIZE 16
#define MAX_SKYLANDER_COUNT 4

#define FPS 1000/24

// LCD TFT
#define SCREEN_W            160
#define SCREEN_H            128
#define CHAR_W              6
#define CHAR_H              8
#define COLS                (SCREEN_W / CHAR_W)
#define HEADER_Y            1
#define LIST_Y              12
#define LIST_ROWS           ((SCREEN_H - LIST_Y) / CHAR_H)


//////////////////////////

extern Player players[2];

extern char sense_counter;
extern bool shutthefuckup;
extern bool send_data_now;
extern FIL *loaded_skylanders[MAX_SKYLANDER_COUNT];
//extern Slot slots[4];



// menu
extern char selected_path[256];
extern int menu_;   //0 : main
                    //1 : player
                    //2 : explorer
extern int player_selected;     //0 : no select
                                //1 : player 1
                                //2 : player 2


extern Folder *root;


extern int just_add;
extern int just_remove;

#endif
